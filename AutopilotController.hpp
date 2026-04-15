#pragma once

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QList>
#include <QMap>
#include "ImageAnalyzer.hpp"
#include "MessierCatalog.hpp"
#include "CoordinateUtils.hpp"

class OriginBackend;
class TelescopeDataProcessor;

class AutopilotController : public QObject {
    Q_OBJECT
public:
    enum State {
        Idle,
        SelectingTarget,
        Slewing,
        Imaging,
        Analyzing,
        AdjustingExposure,
        Refocusing,
        RecoveringError
    };
    Q_ENUM(State)

    struct TargetScore {
        MessierObject object;
        double altitude = 0;       // current altitude in degrees
        double azimuth = 0;
        double moonDistance = 0;    // angular distance from moon in degrees
        double timeToSet = 0;      // hours until below min altitude
        double hourAngle = 0;      // hours from meridian
        double score = 0;          // composite 0-100
        bool visible = false;
    };

    struct MoonPosition {
        double ra = 0;             // degrees
        double dec = 0;            // degrees
        double altitude = 0;       // degrees
        double azimuth = 0;        // degrees
    };

    explicit AutopilotController(OriginBackend *backend, QObject *parent = nullptr);

    void start();
    void pause();
    void stop();

    State state() const { return m_state; }
    QString stateString() const;
    QString currentTargetName() const;
    QList<TargetScore> targetScores() const { return m_targetScores; }
    ImageAnalyzer* imageAnalyzer() { return &m_imageAnalyzer; }

    // Configuration
    void setObserverLocation(double latitude, double longitude);
    void setMinAltitude(double degrees) { m_minAltitude = degrees; }
    void setMinMoonDistance(double degrees) { m_minMoonDistance = degrees; }
    void setTargetFrameCount(int n) { m_targetFrameCount = n; }
    void setExposure(double seconds) { m_currentExposure = seconds; }
    void setISO(int iso) { m_currentISO = iso; }
    void setCandidateTargets(const QList<int> &messierIds);

signals:
    void stateChanged(AutopilotController::State newState);
    void logMessage(const QString &message);
    void targetScoresUpdated();
    void exposureChanged(double seconds, int iso);

public slots:
    void onMountStatusUpdated();
    void onEnvironmentStatusUpdated();
    void onTaskControllerStatusUpdated();
    void onTiffImageDownloaded(const QString &filePath, const QByteArray &imageData,
                               double ra, double dec, double exposure);

private:
    void setState(State s);
    void log(const QString &msg);

    // Target selection
    void updateTargetScores();
    void selectBestTarget();
    double scoreTarget(const TargetScore &t) const;
    double computeTimeToSet(double ra, double dec) const;
    MoonPosition computeMoonPosition() const;
    double angularDistance(double ra1, double dec1, double ra2, double dec2) const;

    // Commanding
    void slewToTarget(const MessierObject &obj);
    void startImaging();
    void stopImaging();
    void adjustExposure();
    void triggerRefocus();

    // Monitoring
    void checkQuality();
    void checkFocusNeeded();

    // Error recovery
    void handleError(const QString &reason);
    void attemptRecovery();

    // Timers
    QTimer m_evaluationTimer;      // periodic target re-evaluation
    QTimer m_recoveryTimer;        // delay before recovery attempt

    // References
    OriginBackend *m_backend = nullptr;
    TelescopeDataProcessor *m_dataProcessor = nullptr;

    // State
    State m_state = Idle;
    bool m_paused = false;
    int m_consecutiveErrors = 0;
    int m_framesThisTarget = 0;
    int m_framesSinceExposureChange = 0;
    double m_lastFocusTemperature = -999;

    // Current target
    MessierObject m_currentTarget;
    bool m_hasTarget = false;

    // Scoring
    QList<TargetScore> m_targetScores;
    QList<int> m_candidateIds;     // empty = use all Messier objects

    // Observer location
    double m_latitude = 0;
    double m_longitude = 0;

    // Configuration
    double m_minAltitude = 20.0;
    double m_minMoonDistance = 15.0;
    int m_targetFrameCount = 60;   // frames per target before moving on

    // Exposure state
    double m_currentExposure = 8.0;
    int m_currentISO = 800;
    static const int EXPOSURE_SETTLE_FRAMES = 5;

    // Exposure limits
    static constexpr double MIN_EXPOSURE = 1.0;
    static constexpr double MAX_EXPOSURE = 30.0;
    static const int MIN_ISO = 100;
    static const int MAX_ISO = 3200;

    // Background ADU targets (16-bit)
    static const int BG_LOW = 800;
    static const int BG_HIGH = 4000;

    // Image analysis
    ImageAnalyzer m_imageAnalyzer;
};
