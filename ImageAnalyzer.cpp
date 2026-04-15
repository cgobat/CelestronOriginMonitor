#include "ImageAnalyzer.hpp"
#include <QtEndian>
#include <QDebug>
#include <cmath>
#include <algorithm>

ImageAnalyzer::ImageAnalyzer(QObject *parent)
    : QObject(parent)
{
}

void ImageAnalyzer::resetStack()
{
    m_accumulator.clear();
    m_width = 0;
    m_height = 0;
    m_frameCount = 0;
    m_totalExposure = 0;
    m_recentStats.clear();
    m_lastStats = FrameStats();
    m_histogram = Histogram();
}

// ---------------------------------------------------------------------------
// TIFF Parsing — handles simple uncompressed 16-bit grayscale TIFFs
// ---------------------------------------------------------------------------

ImageAnalyzer::TiffInfo ImageAnalyzer::parseTiffHeader(const QByteArray &data)
{
    TiffInfo info;
    if (data.size() < 8) return info;

    const uchar *d = reinterpret_cast<const uchar *>(data.constData());

    // Byte order
    if (d[0] == 'I' && d[1] == 'I')
        info.littleEndian = true;
    else if (d[0] == 'M' && d[1] == 'M')
        info.littleEndian = false;
    else
        return info;

    auto read16 = [&](int offset) -> quint16 {
        quint16 v;
        memcpy(&v, d + offset, 2);
        return info.littleEndian ? qFromLittleEndian(v) : qFromBigEndian(v);
    };
    auto read32 = [&](int offset) -> quint32 {
        quint32 v;
        memcpy(&v, d + offset, 4);
        return info.littleEndian ? qFromLittleEndian(v) : qFromBigEndian(v);
    };

    // Magic number
    if (read16(2) != 42) return info;

    // First IFD offset
    quint32 ifdOffset = read32(4);
    if (ifdOffset + 2 > (quint32)data.size()) return info;

    quint16 numEntries = read16(ifdOffset);
    quint32 entryBase = ifdOffset + 2;

    for (int i = 0; i < numEntries; i++) {
        quint32 pos = entryBase + i * 12;
        if (pos + 12 > (quint32)data.size()) break;

        quint16 tag = read16(pos);
        quint16 type = read16(pos + 2);
        quint32 count = read32(pos + 4);
        quint32 valueOffset = read32(pos + 8);

        // For SHORT values stored inline (count=1), value is in lower 2 bytes
        quint16 shortVal = read16(pos + 8);

        switch (tag) {
        case 256: // ImageWidth
            info.width = (type == 3) ? shortVal : valueOffset;
            break;
        case 257: // ImageLength
            info.height = (type == 3) ? shortVal : valueOffset;
            break;
        case 258: // BitsPerSample
            if (count == 1) {
                info.bitsPerSample = shortVal;
            } else {
                // count > 1 (e.g. RGB): values stored at offset pointed to by valueOffset
                if (type == 3 && valueOffset + 2 <= (quint32)data.size())
                    info.bitsPerSample = read16(valueOffset);
                else
                    info.bitsPerSample = shortVal;
            }
            break;
        case 273: // StripOffsets
            info.stripOffset = (type == 3) ? shortVal : valueOffset;
            break;
        case 277: // SamplesPerPixel
            info.samplesPerPixel = shortVal;
            break;
        case 279: // StripByteCounts
            info.stripByteCount = (type == 3) ? shortVal : valueOffset;
            break;
        }
    }

    info.valid = (info.width > 0 && info.height > 0 && info.stripOffset > 0);
    return info;
}

QVector<quint16> ImageAnalyzer::extractPixels(const QByteArray &data, const TiffInfo &info)
{
    int numPixels = info.width * info.height;
    QVector<quint16> pixels(numPixels);

    quint32 expectedBytes = numPixels * (info.bitsPerSample / 8) * info.samplesPerPixel;
    quint32 available = data.size() - info.stripOffset;
    if (available < expectedBytes) {
        qWarning() << "TIFF strip too small:" << available << "need" << expectedBytes;
        return QVector<quint16>();
    }

    const uchar *src = reinterpret_cast<const uchar *>(data.constData()) + info.stripOffset;

    if (info.bitsPerSample == 16 && info.samplesPerPixel == 3) {
        // 16-bit interleaved RGB: read all samples, convert to luminance
        QVector<quint16> lum(numPixels);
        for (int i = 0; i < numPixels; i++) {
            quint16 r, g, b;
            memcpy(&r, src + i * 6, 2);
            memcpy(&g, src + i * 6 + 2, 2);
            memcpy(&b, src + i * 6 + 4, 2);
            if (info.littleEndian) {
                r = qFromLittleEndian(r);
                g = qFromLittleEndian(g);
                b = qFromLittleEndian(b);
            } else {
                r = qFromBigEndian(r);
                g = qFromBigEndian(g);
                b = qFromBigEndian(b);
            }
            lum[i] = qBound(0, (int)(0.299 * r + 0.587 * g + 0.114 * b), 65535);
        }
        return lum;
    } else if (info.bitsPerSample == 16 && info.samplesPerPixel == 1) {
        for (int i = 0; i < numPixels; i++) {
            quint16 v;
            memcpy(&v, src + i * 2, 2);
            pixels[i] = info.littleEndian ? qFromLittleEndian(v) : qFromBigEndian(v);
        }
    } else if (info.bitsPerSample == 8 && info.samplesPerPixel == 1) {
        for (int i = 0; i < numPixels; i++) {
            pixels[i] = src[i] * 257;
        }
    } else if (info.bitsPerSample == 8 && info.samplesPerPixel == 3) {
        // 8-bit RGB: convert to 16-bit luminance
        QVector<quint16> lum(numPixels);
        for (int i = 0; i < numPixels; i++) {
            double r = src[i * 3];
            double g = src[i * 3 + 1];
            double b = src[i * 3 + 2];
            lum[i] = qBound(0, (int)((0.299*r + 0.587*g + 0.114*b) * 257.0), 65535);
        }
        return lum;
    }

    return pixels;
}

// ---------------------------------------------------------------------------
// Frame Analysis
// ---------------------------------------------------------------------------

ImageAnalyzer::Histogram ImageAnalyzer::computeHistogram(const QVector<quint16> &pixels)
{
    Histogram h;
    h.bins.resize(256);
    h.bins.fill(0);
    h.min = 65535;
    h.max = 0;

    double sum = 0;
    double sumSq = 0;
    int n = pixels.size();

    for (int i = 0; i < n; i++) {
        double v = pixels[i];
        int bin = pixels[i] >> 8; // map 16-bit to 8-bit bin
        if (bin > 255) bin = 255;
        h.bins[bin]++;
        sum += v;
        sumSq += v * v;
        if (v < h.min) h.min = v;
        if (v > h.max) h.max = v;
    }

    h.mean = sum / n;
    h.stdDev = std::sqrt(sumSq / n - h.mean * h.mean);
    h.median = estimateMedian(h, n);
    return h;
}

double ImageAnalyzer::estimateMedian(const Histogram &hist, int totalPixels)
{
    int target = totalPixels / 2;
    int cumulative = 0;
    for (int i = 0; i < 256; i++) {
        cumulative += hist.bins[i];
        if (cumulative >= target) {
            // Map bin back to 16-bit range (midpoint of bin)
            return (i + 0.5) * 256.0;
        }
    }
    return 0;
}

double ImageAnalyzer::computeSharpness(const QVector<quint16> &pixels, int w, int h)
{
    // Gradient energy on subsampled grid (every 4th pixel)
    double energy = 0;
    int count = 0;
    int step = 4;

    for (int y = step; y < h - step; y += step) {
        for (int x = step; x < w - step; x += step) {
            double gx = (double)pixels[y * w + x + 1] - pixels[y * w + x - 1];
            double gy = (double)pixels[(y + 1) * w + x] - pixels[(y - 1) * w + x];
            energy += gx * gx + gy * gy;
            count++;
        }
    }

    return count > 0 ? energy / count : 0;
}

ImageAnalyzer::FrameStats ImageAnalyzer::analyzeFrame(const QVector<quint16> &pixels, int w, int h)
{
    FrameStats stats;
    stats.frameNumber = m_frameCount;
    stats.timestamp = QDateTime::currentDateTime();

    Histogram hist = computeHistogram(pixels);
    stats.meanBackground = hist.mean;
    stats.medianBackground = hist.median;
    stats.stdDevBackground = hist.stdDev;
    stats.peakValue = hist.max;
    stats.sharpness = computeSharpness(pixels, w, h);

    // SNR estimate: (peak region - background) / noise
    if (hist.stdDev > 0) {
        stats.snrEstimate = (hist.max - hist.median) / hist.stdDev;
    }

    // Hot pixel count: pixels > mean + 10*stddev
    double hotThreshold = hist.mean + 10.0 * hist.stdDev;
    for (int i = 0; i < pixels.size(); i++) {
        if (pixels[i] > hotThreshold) stats.hotPixelCount++;
    }

    return stats;
}

// ---------------------------------------------------------------------------
// Main entry: add a frame
// ---------------------------------------------------------------------------

void ImageAnalyzer::addFrame(const QByteArray &tiffData, double exposure)
{
    TiffInfo info = parseTiffHeader(tiffData);
    QVector<quint16> pixels;

    if (info.valid) {
        pixels = extractPixels(tiffData, info);
    }

    // Fallback: try QImage if TIFF parsing failed
    if (pixels.isEmpty()) {
        QImage img;
        if (img.loadFromData(tiffData)) {
            QImage gray = img.convertToFormat(QImage::Format_Grayscale8);
            info.width = gray.width();
            info.height = gray.height();
            info.bitsPerSample = 8;
            pixels.resize(info.width * info.height);
            for (int y = 0; y < info.height; y++) {
                const uchar *line = gray.constScanLine(y);
                for (int x = 0; x < info.width; x++) {
                    pixels[y * info.width + x] = line[x] * 257;
                }
            }
        }
    }

    if (pixels.isEmpty()) {
        qWarning() << "ImageAnalyzer: could not parse frame data";
        return;
    }

    int numPixels = info.width * info.height;

    // Initialize accumulator on first frame
    if (m_frameCount == 0) {
        m_width = info.width;
        m_height = info.height;
        m_accumulator.resize(numPixels);
        m_accumulator.fill(0);
    }

    // Check dimensions match
    if (info.width != m_width || info.height != m_height) {
        qWarning() << "ImageAnalyzer: frame size mismatch, resetting stack";
        resetStack();
        m_width = info.width;
        m_height = info.height;
        m_accumulator.resize(numPixels);
        m_accumulator.fill(0);
    }

    // Accumulate
    for (int i = 0; i < numPixels; i++) {
        m_accumulator[i] += pixels[i];
    }
    m_frameCount++;
    m_totalExposure += exposure;

    // Analyze this frame
    m_lastStats = analyzeFrame(pixels, m_width, m_height);
    m_histogram = computeHistogram(pixels);

    // Keep recent stats for trend analysis
    m_recentStats.append(m_lastStats);
    while (m_recentStats.size() > STATS_HISTORY) {
        m_recentStats.removeFirst();
    }

    // Check for quality issues
    if (m_recentStats.size() >= 5 && isSharpnessDeclining(5)) {
        emit qualityAlert("Focus may be degrading - sharpness declining over 5 frames");
    }

    emit frameProcessed(m_lastStats);
    emit stackUpdated(m_frameCount, m_totalExposure);
}

// ---------------------------------------------------------------------------
// Display: auto-stretched stacked image
// ---------------------------------------------------------------------------

QImage ImageAnalyzer::autoStretch(const QVector<double> &accumulator, int w, int h, int frames) const
{
    if (frames == 0 || accumulator.isEmpty())
        return QImage();

    // Compute mean image
    int n = w * h;
    QVector<double> mean(n);
    for (int i = 0; i < n; i++) {
        mean[i] = accumulator[i] / frames;
    }

    // Find 5th and 99.5th percentile for stretch
    QVector<double> sorted = mean;
    std::sort(sorted.begin(), sorted.end());

    double lo = sorted[n * 5 / 1000];      // 0.5th percentile
    double hi = sorted[n * 995 / 1000];     // 99.5th percentile

    if (hi <= lo) {
        lo = sorted.first();
        hi = sorted.last();
    }
    double range = hi - lo;
    if (range < 1.0) range = 1.0;

    // Create 8-bit grayscale image
    QImage img(w, h, QImage::Format_Grayscale8);
    for (int y = 0; y < h; y++) {
        uchar *line = img.scanLine(y);
        for (int x = 0; x < w; x++) {
            double v = (mean[y * w + x] - lo) / range;
            v = qBound(0.0, v, 1.0);
            line[x] = (uchar)(v * 255);
        }
    }
    return img;
}

QImage ImageAnalyzer::getStackedImage() const
{
    return autoStretch(m_accumulator, m_width, m_height, m_frameCount);
}

QImage ImageAnalyzer::getHistogramImage(int w, int h) const
{
    QImage img(w, h, QImage::Format_RGB32);
    img.fill(Qt::black);

    if (m_histogram.bins.isEmpty()) return img;

    // Find max bin count for scaling
    int maxCount = 0;
    for (int i = 0; i < 256; i++) {
        if (m_histogram.bins[i] > maxCount)
            maxCount = m_histogram.bins[i];
    }
    if (maxCount == 0) return img;

    // Use log scale for better visibility
    double logMax = std::log(maxCount + 1);

    for (int x = 0; x < w; x++) {
        int bin = x * 256 / w;
        if (bin >= 256) bin = 255;
        double logVal = std::log(m_histogram.bins[bin] + 1);
        int barHeight = (int)(logVal / logMax * (h - 1));

        for (int y = h - 1; y >= h - 1 - barHeight; y--) {
            if (y >= 0 && y < h) {
                // Color: white with slight blue tint
                img.setPixel(x, y, qRgb(200, 200, 255));
            }
        }
    }

    return img;
}

// ---------------------------------------------------------------------------
// Trend analysis
// ---------------------------------------------------------------------------

bool ImageAnalyzer::isSharpnessDeclining(int windowSize) const
{
    if (m_recentStats.size() < windowSize) return false;

    int start = m_recentStats.size() - windowSize;
    int declines = 0;
    for (int i = start + 1; i < m_recentStats.size(); i++) {
        if (m_recentStats[i].sharpness < m_recentStats[i - 1].sharpness)
            declines++;
    }
    // Declining if >80% of consecutive pairs show decrease
    return declines >= (windowSize - 1) * 4 / 5;
}

double ImageAnalyzer::averageSNR(int lastN) const
{
    if (m_recentStats.isEmpty()) return 0;

    int count = qMin(lastN, m_recentStats.size());
    double sum = 0;
    for (int i = m_recentStats.size() - count; i < m_recentStats.size(); i++) {
        sum += m_recentStats[i].snrEstimate;
    }
    return sum / count;
}
