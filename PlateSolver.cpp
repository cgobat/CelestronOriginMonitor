#include "PlateSolver.hpp"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>
#include <cmath>

PlateSolver::PlateSolver(QObject *parent)
    : QObject(parent)
{
    // Default index folder paths (platform-specific)
#ifdef Q_OS_WIN
    // Common Windows install locations for astrometry index files
    m_indexFolderPaths << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/astrometry"
                       << "C:/Program Files/Astrometry/data"
                       << QCoreApplication::applicationDirPath() + "/astrometry";
#elif defined(Q_OS_MACOS)
    m_indexFolderPaths << "/opt/homebrew/Cellar/astrometry-net/0.97/data"
                       << "/usr/local/share/astrometry";
#else
    m_indexFolderPaths << "/usr/share/astrometry"
                       << "/usr/local/share/astrometry";
#endif
}

PlateSolver::~PlateSolver()
{
    if (m_solver) {
        m_solver->abort();
        delete m_solver;
    }
}

void PlateSolver::setIndexFolderPaths(const QStringList &paths)
{
    m_indexFolderPaths = paths;
}

void PlateSolver::setSearchScale(double fovLowArcmin, double fovHighArcmin)
{
    m_fovLowArcmin = fovLowArcmin;
    m_fovHighArcmin = fovHighArcmin;
}

void PlateSolver::solveImage(const QByteArray &tiffData, double hintRaDeg, double hintDecDeg)
{
    Q_UNUSED(hintRaDeg);
    Q_UNUSED(hintDecDeg);

    if (m_solving) {
        emit logMessage("Plate solve already in progress, ignoring request");
        return;
    }

    // Parse TIFF using ImageAnalyzer's static methods
    ImageAnalyzer::TiffInfo tiffInfo = ImageAnalyzer::parseTiffHeader(tiffData);
    if (!tiffInfo.valid) {
        emit solveFailed("Failed to parse TIFF header");
        return;
    }

    m_pixelBuffer = ImageAnalyzer::extractPixels(tiffData, tiffInfo);
    if (m_pixelBuffer.isEmpty()) {
        emit solveFailed("Failed to extract pixels from TIFF");
        return;
    }

    m_imageWidth = tiffInfo.width;
    m_imageHeight = tiffInfo.height;

    int effectiveWidth = tiffInfo.width;
    int effectiveHeight = tiffInfo.height;

    emit logMessage(QString("Blind plate solving %1x%2 image (scale %3-%4 arcmin)")
                    .arg(effectiveWidth).arg(effectiveHeight)
                    .arg(m_fovLowArcmin, 0, 'f', 0).arg(m_fovHighArcmin, 0, 'f', 0));

    // Build FITSImage::Statistic for StellarSolver
    FITSImage::Statistic stats;
    stats.width = effectiveWidth;
    stats.height = effectiveHeight;
    stats.channels = 1;
    stats.dataType = 20;          // TUSHORT (cfitsio type code)
    stats.bytesPerPixel = 2;
    stats.samples_per_channel = effectiveWidth * effectiveHeight;
    stats.ndim = 2;

    // Clean up previous solver
    if (m_solver) {
        m_solver->abort();
        delete m_solver;
        m_solver = nullptr;
    }

    // Create StellarSolver with image data
    m_solver = new StellarSolver(SSolver::SOLVE, stats,
                                  reinterpret_cast<const uint8_t*>(m_pixelBuffer.constData()),
                                  this);

    // Configure solver parameters
    m_solver->setParameterProfile(SSolver::Parameters::DEFAULT);
    m_solver->setIndexFolderPaths(m_indexFolderPaths);

    m_solver->setSearchScale(m_fovLowArcmin, m_fovHighArcmin, SSolver::ARCMIN_WIDTH);

    // Inclinometer hint disabled — blind solve works reliably with index-4108.
    // TODO: re-enable once verified that the Dec constraint doesn't reject valid solutions.
    m_incAltDeg = NAN;
    m_incLatDeg = NAN;

    // Connect completion signal
    connect(m_solver, &StellarSolver::ready, this, &PlateSolver::onSolverReady);

    // Start async solve
    m_solving = true;
    m_solveTimer.start();
    m_solver->start();
}

void PlateSolver::setInclinometerHint(double altitudeDeg, double latitudeDeg, double longitudeDeg)
{
    m_incAltDeg = altitudeDeg;
    m_incLatDeg = latitudeDeg;
    m_incLonDeg = longitudeDeg;
}

void PlateSolver::abort()
{
    if (m_solver && m_solving) {
        m_solver->abort();
        m_solving = false;
        emit logMessage("Plate solve aborted");
    }
}

void PlateSolver::onSolverReady()
{
    m_solving = false;
    double elapsedMs = m_solveTimer.elapsed();

    if (!m_solver) {
        emit solveFailed("Solver object destroyed unexpectedly");
        return;
    }

    if (m_solver->failed() || !m_solver->solvingDone()) {
        emit logMessage(QString("Plate solve failed after %1 ms").arg(elapsedMs, 0, 'f', 0));
        emit solveFailed("Plate solving failed - no solution found");
        return;
    }

    // Extract solution
    FITSImage::Solution solution = m_solver->getSolution();

    PlateSolveResult result;
    result.success = true;
    result.ra = solution.ra;
    result.dec = solution.dec;
    result.orientation = solution.orientation;
    result.pixscale = solution.pixscale;
    result.fieldWidth = solution.fieldWidth;
    result.fieldHeight = solution.fieldHeight;
    result.raError = solution.raError;
    result.decError = solution.decError;
    result.parity = solution.parity;
    result.timestamp = QDateTime::currentDateTime();
    result.solveTimeMs = elapsedMs;

    // Get star list
    result.stars = m_solver->getStarListFromSolve();
    result.numStarsDetected = m_solver->getNumStarsFound();

    m_lastResult = result;

    emit logMessage(QString("Plate solve OK in %1 ms: RA=%2 Dec=%3 orient=%4 scale=%5\"/px  %6 stars")
                    .arg(elapsedMs, 0, 'f', 0)
                    .arg(result.ra, 0, 'f', 5)
                    .arg(result.dec, 0, 'f', 5)
                    .arg(result.orientation, 0, 'f', 2)
                    .arg(result.pixscale, 0, 'f', 3)
                    .arg(result.numStarsDetected));

    emit solveCompleted(result);
}
