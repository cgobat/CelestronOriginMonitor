#pragma once

#include <QObject>
#include <QTimer>
#include <QList>
#include <QString>
#include "PlateSolver.hpp"
#include "MountModel.hpp"

class OriginBackend;
class TelescopeDataProcessor;

// Bright navigation star for alignment
struct AlignmentStar {
    QString name;
    double raDeg;    // J2000
    double decDeg;   // J2000
    double magnitude;
};

class AlignmentController : public QObject
{
    Q_OBJECT
public:
    enum AlignState {
        Idle,
        Starting,
        InitialCapture,        // take snapshot to determine current position
        InitialSolve,          // plate solve the initial image
        SelectingStar,         // pick next alignment star
        SlewingToStar,         // relative slew towards target star
        WaitingForCorrection,  // waiting for correction slew timer
        SettlingAfterSlew,     // waiting for vibrations to damp
        TakingSnapshot,        // capture image for solve
        WaitingForImage,       // waiting for TIFF download
        PlateSolving,          // running plate solver
        EvaluatingResult,      // check if close enough or need another iteration
        SyncingMount,          // sync mount to solved position
        AdvancingToNext,       // move to next star
        BuildingModel,         // fit the pointing model
        Complete,
        Error
    };
    Q_ENUM(AlignState)

    struct StarStatus {
        AlignmentStar star;
        enum Status { Pending, Slewing, Solving, Solved, Failed, Skipped };
        Status status = Pending;
        double errorArcsec = 0;
        double solvedRaDeg = 0;
        double solvedDecDeg = 0;
    };

    explicit AlignmentController(OriginBackend *backend, QObject *parent = nullptr);

    void startAlignment(int numStars = 6);
    void abort();
    void oneShotSolve();

    AlignState state() const { return m_state; }
    QString stateString() const;
    PlateSolver* plateSolver() { return &m_plateSolver; }
    MountModel* mountModel() { return &m_mountModel; }
    QList<StarStatus> starStatuses() const { return m_starStatuses; }
    int completedStars() const { return m_completedStars; }
    int totalStars() const { return m_numStarsRequested; }

signals:
    void stateChanged(AlignmentController::AlignState newState);
    void alignmentProgress(int completed, int total, const QString &starName);
    void plateSolveCompleted(const PlateSolveResult &result);
    void modelReady(const ModelCoefficients &coeffs);
    void logMessage(const QString &msg);
    void starStatusesUpdated();

public slots:
    void onMountStatusUpdated();
    void onTiffImageDownloaded(const QString &filePath, const QByteArray &imageData,
                               double ra, double dec, double exposure);

private slots:
    void onPlateSolveCompleted(const PlateSolveResult &result);
    void onPlateSolveFailed(const QString &reason);
    void onSettleTimeout();
    void onCorrectionComplete();

private:
    void setState(AlignState s);
    void log(const QString &msg);

    // Star selection
    void buildVisibleStarList();
    void selectNextStar();
    double angularSeparation(double ra1, double dec1, double ra2, double dec2) const;

    // Coordinate helpers
    void raDecToAltAz(double raDeg, double decDeg, double &altDeg, double &azDeg) const;

    // Alignment sequence steps
    void navigateToStar();          // compute offset and start relative slew
    void takeAlignmentSnapshot();
    void evaluateSolveResult(const PlateSolveResult &result);
    void syncMountToSolution(const PlateSolveResult &result);
    void advanceToNextStar();
    void buildFinalModel();

    // Static bright star catalog
    static QList<AlignmentStar> brightStarCatalog();

    // References
    OriginBackend *m_backend = nullptr;
    TelescopeDataProcessor *m_dataProcessor = nullptr;

    // Components
    PlateSolver m_plateSolver;
    MountModel m_mountModel;

    // State
    AlignState m_state = Idle;
    bool m_oneShotMode = false;
    int m_numStarsRequested = 6;
    int m_completedStars = 0;
    int m_currentStarIndex = -1;
    int m_retryCount = 0;
    int m_correctionIteration = 0;
    static constexpr int MAX_CORRECTION_ITERATIONS = 5;
    static constexpr double CORRECTION_THRESHOLD_ARCSEC = 30.0;

    // Current known position (from last plate solve)
    double m_currentRaDeg = 0;
    double m_currentDecDeg = 0;
    bool m_positionKnown = false;

    // Star tracking
    QList<AlignmentStar> m_visibleStars;
    QList<StarStatus> m_starStatuses;
    QList<int> m_selectedIndices;

    // Observer location
    double m_latitude = 0;
    double m_longitude = 0;
    bool m_hasLocation = false;

    // Timers
    QTimer m_settleTimer;
    QTimer m_timeoutTimer;
    QTimer m_correctionTimer;  // times the relative slew duration

    // Image metadata from last notification
    double m_lastImageRaDeg = 0;
    double m_lastImageDecDeg = 0;
};
