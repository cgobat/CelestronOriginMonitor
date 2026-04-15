#pragma once

#include <QObject>
#include <QImage>
#include <QVector>
#include <QDateTime>
#include <QList>

class ImageAnalyzer : public QObject {
    Q_OBJECT
public:
    struct FrameStats {
        double meanBackground = 0;
        double medianBackground = 0;
        double stdDevBackground = 0;
        double peakValue = 0;
        double snrEstimate = 0;
        double sharpness = 0;       // gradient energy (higher = sharper)
        int hotPixelCount = 0;
        int frameNumber = 0;
        QDateTime timestamp;
    };

    struct Histogram {
        QVector<int> bins;          // 256 bins
        double min = 0;
        double max = 0;
        double mean = 0;
        double median = 0;
        double stdDev = 0;
    };

    explicit ImageAnalyzer(QObject *parent = nullptr);

    void resetStack();
    void addFrame(const QByteArray &tiffData, double exposure);

    FrameStats lastFrameStats() const { return m_lastStats; }
    Histogram currentHistogram() const { return m_histogram; }
    QImage getStackedImage() const;
    QImage getHistogramImage(int w, int h) const;
    int frameCount() const { return m_frameCount; }
    double totalExposure() const { return m_totalExposure; }

    // Trend analysis
    bool isSharpnessDeclining(int windowSize = 5) const;
    double averageSNR(int lastN = 10) const;

    // TIFF parsing (public static for reuse by PlateSolver)
    struct TiffInfo {
        int width = 0;
        int height = 0;
        int bitsPerSample = 16;
        int samplesPerPixel = 1;
        quint32 stripOffset = 0;
        quint32 stripByteCount = 0;
        bool littleEndian = true;
        bool valid = false;
    };

    static TiffInfo parseTiffHeader(const QByteArray &data);
    static QVector<quint16> extractPixels(const QByteArray &data, const TiffInfo &info);

signals:
    void frameProcessed(const ImageAnalyzer::FrameStats &stats);
    void stackUpdated(int totalFrames, double totalExposure);
    void qualityAlert(const QString &issue);

private:

    // Analysis
    FrameStats analyzeFrame(const QVector<quint16> &pixels, int w, int h);
    Histogram computeHistogram(const QVector<quint16> &pixels);
    double estimateMedian(const Histogram &hist, int totalPixels);
    double computeSharpness(const QVector<quint16> &pixels, int w, int h);

    // Display
    QImage autoStretch(const QVector<double> &accumulator, int w, int h, int frames) const;

    // Accumulator
    QVector<double> m_accumulator;
    int m_width = 0;
    int m_height = 0;
    int m_frameCount = 0;
    double m_totalExposure = 0;

    // Stats history
    FrameStats m_lastStats;
    Histogram m_histogram;
    QList<FrameStats> m_recentStats;
    static const int STATS_HISTORY = 20;
};
