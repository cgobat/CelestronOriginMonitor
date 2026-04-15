#pragma once

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QElapsedTimer>
#include <QVector>
#include <stellarsolver.h>

#include "ImageAnalyzer.hpp"

struct PlateSolveResult {
    bool success = false;
    double ra = 0;              // degrees (J2000)
    double dec = 0;             // degrees (J2000)
    double orientation = 0;     // degrees from North
    double pixscale = 0;        // arcsec/pixel
    double fieldWidth = 0;      // arcminutes
    double fieldHeight = 0;     // arcminutes
    double raError = 0;         // arcsec (vs search hint)
    double decError = 0;        // arcsec (vs search hint)
    FITSImage::Parity parity = FITSImage::BOTH;
    QList<FITSImage::Star> stars;
    QDateTime timestamp;
    double solveTimeMs = 0;
    int numStarsDetected = 0;
};

class PlateSolver : public QObject
{
    Q_OBJECT
public:
    explicit PlateSolver(QObject *parent = nullptr);
    ~PlateSolver();

    void solveImage(const QByteArray &tiffData, double hintRaDeg, double hintDecDeg);
    void abort();

    PlateSolveResult lastResult() const { return m_lastResult; }
    bool isSolving() const { return m_solving; }

    // Configuration
    void setIndexFolderPaths(const QStringList &paths);
    void setSearchScale(double fovLowArcmin, double fovHighArcmin);
    void setTimeoutSeconds(int seconds) { m_timeoutSeconds = seconds; }

    // Inclinometer hint: altitude (degrees) from the orientation sensor
    // combined with observer latitude/longitude to constrain the Dec search range.
    // Set before calling solveImage(). Pass NaN to disable.
    void setInclinometerHint(double altitudeDeg, double latitudeDeg, double longitudeDeg = 0.0);

signals:
    void solveCompleted(const PlateSolveResult &result);
    void solveFailed(const QString &reason);
    void logMessage(const QString &msg);

private slots:
    void onSolverReady();

private:
    StellarSolver *m_solver = nullptr;
    PlateSolveResult m_lastResult;
    bool m_solving = false;
    QElapsedTimer m_solveTimer;

    // Pixel buffer kept alive while solver runs
    QVector<quint16> m_pixelBuffer;
    int m_imageWidth = 0;
    int m_imageHeight = 0;

    // Configuration
    QStringList m_indexFolderPaths;
    double m_fovLowArcmin = 60.0;   // Origin FOV ~75' wide
    double m_fovHighArcmin = 90.0;
    int m_timeoutSeconds = 30;

    // Inclinometer hint
    double m_incAltDeg = NAN;   // NaN = no hint
    double m_incLatDeg = NAN;
    double m_incLonDeg = 0.0;
};
