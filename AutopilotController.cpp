#include "AutopilotController.hpp"
#include "OriginBackend.hpp"
#include "TelescopeDataProcessor.hpp"
#include "TelescopeData.hpp"
#include <QJsonObject>
#include <QtMath>
#include <QDebug>

AutopilotController::AutopilotController(OriginBackend *backend, QObject *parent)
    : QObject(parent)
    , m_backend(backend)
    , m_dataProcessor(backend->getDataProcessor())
{
    // Connect to telescope status signals
    connect(m_dataProcessor, &TelescopeDataProcessor::mountStatusUpdated,
            this, &AutopilotController::onMountStatusUpdated);
    connect(m_dataProcessor, &TelescopeDataProcessor::environmentStatusUpdated,
            this, &AutopilotController::onEnvironmentStatusUpdated);
    connect(m_dataProcessor, &TelescopeDataProcessor::taskControllerStatusUpdated,
            this, &AutopilotController::onTaskControllerStatusUpdated);
    connect(m_backend, &OriginBackend::tiffImageDownloaded,
            this, &AutopilotController::onTiffImageDownloaded);

    // Periodic target score re-evaluation (every 5 minutes)
    m_evaluationTimer.setInterval(300000);
    connect(&m_evaluationTimer, &QTimer::timeout, this, [this]() {
        if (m_state != Idle) {
            updateTargetScores();
            emit targetScoresUpdated();
        }
    });

    // Recovery timer (10 second delay)
    m_recoveryTimer.setSingleShot(true);
    m_recoveryTimer.setInterval(10000);
    connect(&m_recoveryTimer, &QTimer::timeout, this, &AutopilotController::attemptRecovery);
}

// ---------------------------------------------------------------------------
// State management
// ---------------------------------------------------------------------------

void AutopilotController::setState(State s)
{
    if (m_state == s) return;
    m_state = s;
    log(QString("State -> %1").arg(stateString()));
    emit stateChanged(s);
}

QString AutopilotController::stateString() const
{
    switch (m_state) {
    case Idle:              return "Idle";
    case SelectingTarget:   return "Selecting Target";
    case Slewing:           return "Slewing";
    case Imaging:           return "Imaging";
    case Analyzing:         return "Analyzing";
    case AdjustingExposure: return "Adjusting Exposure";
    case Refocusing:        return "Refocusing";
    case RecoveringError:   return "Recovering";
    }
    return "Unknown";
}

QString AutopilotController::currentTargetName() const
{
    if (!m_hasTarget) return QString();
    return m_currentTarget.name;
}

void AutopilotController::log(const QString &msg)
{
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString full = QString("[%1] %2").arg(ts, msg);
    qDebug().noquote() << "Autopilot:" << full;
    emit logMessage(full);
}

// ---------------------------------------------------------------------------
// Start / Pause / Stop
// ---------------------------------------------------------------------------

void AutopilotController::start()
{
    if (m_state != Idle) {
        if (m_paused) {
            m_paused = false;
            log("Resumed");
            // Continue from where we were
            if (m_state == Imaging)
                startImaging();
            return;
        }
        return;
    }

    log("Starting autopilot");
    m_consecutiveErrors = 0;
    m_paused = false;

    // Get observer location from mount data (telescope sends radians)
    const TelescopeData &data = m_dataProcessor->getData();
    if (data.mount.latitude != 0 || data.mount.longitude != 0) {
        m_latitude = data.mount.latitude * RAD_TO_DEG;
        m_longitude = data.mount.longitude * RAD_TO_DEG;
        log(QString("Observer: lat %1, lon %2").arg(m_latitude, 0, 'f', 2).arg(m_longitude, 0, 'f', 2));
    }

    m_evaluationTimer.start();
    updateTargetScores();
    emit targetScoresUpdated();
    selectBestTarget();
}

void AutopilotController::pause()
{
    if (m_state == Idle) return;
    m_paused = true;
    log("Paused");
    if (m_state == Imaging) {
        stopImaging();
    }
}

void AutopilotController::stop()
{
    if (m_state == Idle) return;
    log("Stopping autopilot");
    m_evaluationTimer.stop();
    m_recoveryTimer.stop();
    if (m_state == Imaging) {
        stopImaging();
    }
    m_hasTarget = false;
    m_imageAnalyzer.resetStack();
    setState(Idle);
}

// ---------------------------------------------------------------------------
// Target scoring
// ---------------------------------------------------------------------------

void AutopilotController::setCandidateTargets(const QList<int> &messierIds)
{
    m_candidateIds = messierIds;
}

void AutopilotController::setObserverLocation(double latitude, double longitude)
{
    m_latitude = latitude;
    m_longitude = longitude;
}

AutopilotController::MoonPosition AutopilotController::computeMoonPosition() const
{
    // Simplified Meeus lunar ephemeris (~2 degree accuracy)
    QDateTime utc = QDateTime::currentDateTimeUtc();
    double jd = CoordinateUtils::computeJulianDay(
        utc.date().year(), utc.date().month(), utc.date().day(),
        utc.time().hour(), utc.time().minute(), utc.time().second());

    double T = (jd - 2451545.0) / 36525.0;  // centuries since J2000

    // Mean elements (degrees)
    double Lp = fmod(218.3165 + 481267.8813 * T, 360.0);  // mean longitude
    double M  = fmod(357.5291 + 35999.0503 * T, 360.0);   // sun mean anomaly
    double Mp = fmod(134.9634 + 477198.8676 * T, 360.0);   // moon mean anomaly
    double D  = fmod(297.8502 + 445267.1115 * T, 360.0);   // mean elongation
    double F  = fmod(93.2720 + 483202.0175 * T, 360.0);    // argument of latitude

    double LpR = Lp * DEG_TO_RAD;
    double MR  = M  * DEG_TO_RAD;
    double MpR = Mp * DEG_TO_RAD;
    double DR  = D  * DEG_TO_RAD;
    double FR  = F  * DEG_TO_RAD;

    // Ecliptic longitude and latitude (simplified)
    double lon = Lp
        + 6.289 * sin(MpR)
        - 1.274 * sin(MpR - 2.0 * DR)
        + 0.658 * sin(2.0 * DR)
        + 0.214 * sin(2.0 * MpR)
        - 0.186 * sin(MR)
        - 0.114 * sin(2.0 * FR);

    double lat = 5.128 * sin(FR)
        + 0.281 * sin(MpR + FR)
        - 0.278 * sin(MpR - FR)
        - 0.173 * sin(FR - 2.0 * DR);

    // Ecliptic to equatorial
    double obliquity = 23.44 * DEG_TO_RAD;
    double lonR = lon * DEG_TO_RAD;
    double latR = lat * DEG_TO_RAD;

    double sinLon = sin(lonR);
    double cosLon = cos(lonR);
    double sinLat = sin(latR);
    double cosLat = cos(latR);
    double sinObl = sin(obliquity);
    double cosObl = cos(obliquity);

    double ra = atan2(sinLon * cosObl - tan(latR) * sinObl, cosLon);
    double dec = asin(sinLat * cosObl + cosLat * sinObl * sinLon);

    MoonPosition moon;
    moon.ra = fmod(ra * RAD_TO_DEG + 360.0, 360.0);
    moon.dec = dec * RAD_TO_DEG;

    // Compute alt/az for the moon (raDecToAltAz takes all radians)
    double lst = CoordinateUtils::localSiderealTime(m_longitude, jd);
    double moonRaRad = moon.ra * DEG_TO_RAD;
    double moonDecRad = moon.dec * DEG_TO_RAD;
    double latRad = m_latitude * DEG_TO_RAD;
    double lonRad = m_longitude * DEG_TO_RAD;
    auto altAz = CoordinateUtils::raDecToAltAz(moonRaRad, moonDecRad, latRad, lonRad, lst);
    moon.altitude = std::get<0>(altAz) * RAD_TO_DEG;
    moon.azimuth = std::get<1>(altAz) * RAD_TO_DEG;

    return moon;
}

double AutopilotController::angularDistance(double ra1, double dec1, double ra2, double dec2) const
{
    // Haversine on the sphere; inputs in degrees
    double r1 = ra1 * DEG_TO_RAD, d1 = dec1 * DEG_TO_RAD;
    double r2 = ra2 * DEG_TO_RAD, d2 = dec2 * DEG_TO_RAD;
    double dDec = d2 - d1;
    double dRa  = r2 - r1;
    double a = sin(dDec / 2) * sin(dDec / 2) +
               cos(d1) * cos(d2) * sin(dRa / 2) * sin(dRa / 2);
    return 2.0 * asin(qMin(1.0, sqrt(a))) * RAD_TO_DEG;
}

double AutopilotController::computeTimeToSet(double ra, double dec) const
{
    // Estimate hours until target drops below minimum altitude
    // by sampling future positions at 15-minute intervals
    QDateTime utc = QDateTime::currentDateTimeUtc();
    double raRad = ra * DEG_TO_RAD;
    double decRad = dec * DEG_TO_RAD;
    double latRad = m_latitude * DEG_TO_RAD;
    double lonRad = m_longitude * DEG_TO_RAD;
    double minAltRad = m_minAltitude * DEG_TO_RAD;

    for (int step = 0; step < 48; step++) {  // up to 12 hours ahead
        QDateTime future = utc.addSecs(step * 900);
        double jd = CoordinateUtils::computeJulianDay(
            future.date().year(), future.date().month(), future.date().day(),
            future.time().hour(), future.time().minute(), future.time().second());
        double lst = CoordinateUtils::localSiderealTime(m_longitude, jd);
        auto altAz = CoordinateUtils::raDecToAltAz(raRad, decRad, latRad, lonRad, lst);
        double alt = std::get<0>(altAz);
        if (alt < minAltRad)
            return step * 0.25;  // hours
    }
    return 12.0;  // still up after 12 hours
}

double AutopilotController::scoreTarget(const TargetScore &t) const
{
    if (!t.visible) return 0;

    // Altitude component (35%): 0 below 20, linear to 100 at 70+
    double altScore = 0;
    if (t.altitude >= 70) altScore = 100;
    else if (t.altitude >= m_minAltitude)
        altScore = (t.altitude - m_minAltitude) / (70.0 - m_minAltitude) * 100.0;

    // Moon distance (25%): 0 below 15, linear to 100 at 90+
    double moonScore = 0;
    if (t.moonDistance >= 90) moonScore = 100;
    else if (t.moonDistance >= m_minMoonDistance)
        moonScore = (t.moonDistance - m_minMoonDistance) / (90.0 - m_minMoonDistance) * 100.0;

    // Time to set (20%): 0 below 0.5h, linear to 100 at 4h+
    double timeScore = 0;
    if (t.timeToSet >= 4.0) timeScore = 100;
    else if (t.timeToSet >= 0.5)
        timeScore = (t.timeToSet - 0.5) / 3.5 * 100.0;

    // Meridian proximity (10%): prefer targets near transit (HA close to 0)
    double haAbs = fabs(t.hourAngle);
    double meridianScore = 100.0 * qMax(0.0, 1.0 - haAbs / 6.0);

    // Brightness penalty (10%): dimmer targets first (they need more time)
    double magScore = 0;
    if (t.object.magnitude >= 10.0) magScore = 100;
    else if (t.object.magnitude >= 5.0)
        magScore = (t.object.magnitude - 5.0) / 5.0 * 100.0;

    return 0.35 * altScore + 0.25 * moonScore + 0.20 * timeScore
         + 0.10 * meridianScore + 0.10 * magScore;
}

void AutopilotController::updateTargetScores()
{
    QList<MessierObject> candidates;
    if (m_candidateIds.isEmpty()) {
        candidates = MessierCatalog::getAllObjects();
    } else {
        for (int id : m_candidateIds)
            candidates.append(MessierCatalog::getObjectById(id));
    }

    QDateTime utc = QDateTime::currentDateTimeUtc();
    double jd = CoordinateUtils::computeJulianDay(
        utc.date().year(), utc.date().month(), utc.date().day(),
        utc.time().hour(), utc.time().minute(), utc.time().second());
    double lst = CoordinateUtils::localSiderealTime(m_longitude, jd);

    MoonPosition moon = computeMoonPosition();

    double latRad = m_latitude * DEG_TO_RAD;
    double lonRad = m_longitude * DEG_TO_RAD;

    m_targetScores.clear();
    for (const MessierObject &obj : candidates) {
        TargetScore ts;
        ts.object = obj;

        double raRad = obj.sky_position.ra_deg * DEG_TO_RAD;
        double decRad = obj.sky_position.dec_deg * DEG_TO_RAD;
        auto altAz = CoordinateUtils::raDecToAltAz(raRad, decRad, latRad, lonRad, lst);

        ts.altitude = std::get<0>(altAz) * RAD_TO_DEG;
        ts.azimuth = std::get<1>(altAz) * RAD_TO_DEG;
        ts.visible = (ts.altitude >= m_minAltitude);

        // Hour angle in hours (LST is radians, RA is radians)
        double haRad = std::get<2>(altAz);
        ts.hourAngle = haRad * 12.0 / M_PI;  // radians to hours
        if (ts.hourAngle > 12) ts.hourAngle -= 24;
        if (ts.hourAngle < -12) ts.hourAngle += 24;

        ts.moonDistance = angularDistance(
            obj.sky_position.ra_deg, obj.sky_position.dec_deg,
            moon.ra, moon.dec);

        ts.timeToSet = computeTimeToSet(obj.sky_position.ra_deg, obj.sky_position.dec_deg);
        ts.score = scoreTarget(ts);

        m_targetScores.append(ts);
    }

    // Sort by altitude descending
    std::sort(m_targetScores.begin(), m_targetScores.end(),
              [](const TargetScore &a, const TargetScore &b) {
        return a.altitude > b.altitude;
    });
}

void AutopilotController::selectBestTarget()
{
    setState(SelectingTarget);
    updateTargetScores();
    emit targetScoresUpdated();

    for (const TargetScore &ts : m_targetScores) {
        if (ts.score > 0 && ts.visible) {
            m_currentTarget = ts.object;
            m_hasTarget = true;
            m_framesThisTarget = 0;
            m_imageAnalyzer.resetStack();
            log(QString("Selected %1 (%2) - score %.0f, alt %.1f%3, moon dist %.1f%3")
                .arg(ts.object.name, ts.object.common_name)
                .arg(ts.score).arg(ts.altitude)
                .arg(QChar(0x00B0))
                .arg(ts.moonDistance)
                .arg(QChar(0x00B0)));
            slewToTarget(m_currentTarget);
            return;
        }
    }

    log("No suitable targets found above minimum altitude");
    stop();
}

// ---------------------------------------------------------------------------
// Commanding
// ---------------------------------------------------------------------------

void AutopilotController::slewToTarget(const MessierObject &obj)
{
    setState(Slewing);
    double raHours = obj.sky_position.ra_deg / 15.0;
    double dec = obj.sky_position.dec_deg;
    log(QString("Slewing to %1 (RA %2h, Dec %3%4)")
        .arg(obj.name)
        .arg(raHours, 0, 'f', 3)
        .arg(dec, 0, 'f', 2)
        .arg(QChar(0x00B0)));
    m_backend->gotoPosition(raHours, dec);
}

void AutopilotController::startImaging()
{
    setState(Imaging);
    m_framesSinceExposureChange = 0;

    // Set exposure and ISO first
    m_backend->setCameraExposure(m_currentExposure);
    m_backend->setCameraISO(m_currentISO);

    // Start imaging via TaskController
    QJsonObject params;
    params["Name"] = m_currentTarget.name;
    params["ExposureTime"] = m_currentExposure;
    params["ISO"] = m_currentISO;
    params["Count"] = m_targetFrameCount;
    m_backend->sendCommand("RunImaging", "TaskController", params);

    log(QString("Imaging %1 - %2s ISO %3, target %4 frames")
        .arg(m_currentTarget.name)
        .arg(m_currentExposure)
        .arg(m_currentISO)
        .arg(m_targetFrameCount));
}

void AutopilotController::stopImaging()
{
    m_backend->sendCommand("CancelImaging", "TaskController");
    log("Imaging cancelled");
}

void AutopilotController::adjustExposure()
{
    setState(AdjustingExposure);
    ImageAnalyzer::FrameStats stats = m_imageAnalyzer.lastFrameStats();
    double bg = stats.medianBackground;

    double oldExposure = m_currentExposure;
    int oldISO = m_currentISO;

    if (bg < BG_LOW) {
        // Too dark: increase ISO first (up to max), then exposure
        if (m_currentISO < MAX_ISO) {
            m_currentISO = qMin(m_currentISO * 2, MAX_ISO);
        } else if (m_currentExposure < MAX_EXPOSURE) {
            m_currentExposure = qMin(m_currentExposure * 1.5, MAX_EXPOSURE);
        }
    } else if (bg > BG_HIGH) {
        // Too bright: decrease exposure first, then ISO
        if (m_currentExposure > MIN_EXPOSURE) {
            m_currentExposure = qMax(m_currentExposure / 1.5, MIN_EXPOSURE);
        } else if (m_currentISO > MIN_ISO) {
            m_currentISO = qMax(m_currentISO / 2, MIN_ISO);
        }
    }

    if (m_currentExposure != oldExposure || m_currentISO != oldISO) {
        log(QString("Exposure %1s->%2s, ISO %3->%4 (background ADU: %5)")
            .arg(oldExposure).arg(m_currentExposure)
            .arg(oldISO).arg(m_currentISO)
            .arg(bg, 0, 'f', 0));
        emit exposureChanged(m_currentExposure, m_currentISO);

        // Stop current imaging, apply new settings, restart
        stopImaging();
        m_framesSinceExposureChange = 0;
        // Small delay then restart
        QTimer::singleShot(2000, this, [this]() {
            if (m_state == AdjustingExposure && !m_paused)
                startImaging();
        });
    } else {
        // No change needed, continue imaging
        setState(Imaging);
    }
}

void AutopilotController::triggerRefocus()
{
    setState(Refocusing);
    log("Triggering auto-focus");
    m_backend->sendCommand("CancelImaging", "TaskController");

    // Send focus command
    QJsonObject params;
    m_backend->sendCommand("RunAutoFocus", "Focuser", params);

    // Focus completion is detected via taskControllerStatusUpdated
}

// ---------------------------------------------------------------------------
// Status monitoring slots
// ---------------------------------------------------------------------------

void AutopilotController::onMountStatusUpdated()
{
    if (m_paused) return;

    const TelescopeData &data = m_dataProcessor->getData();

    // Update observer location if available (telescope sends radians)
    if (m_latitude == 0 && m_longitude == 0) {
        if (data.mount.latitude != 0 || data.mount.longitude != 0) {
            m_latitude = data.mount.latitude * RAD_TO_DEG;
            m_longitude = data.mount.longitude * RAD_TO_DEG;
        }
    }

    // Detect slew completion
    if (m_state == Slewing && data.mount.isGotoOver) {
        log("Slew complete");
        m_backend->setTracking(true);
        startImaging();
    }
}

void AutopilotController::onEnvironmentStatusUpdated()
{
    if (m_paused || m_state == Idle) return;

    const TelescopeData &data = m_dataProcessor->getData();
    double temp = data.environment.ambientTemperature;

    // Check if temperature has drifted enough to warrant refocus
    if (m_lastFocusTemperature > -900) {
        double drift = fabs(temp - m_lastFocusTemperature);
        if (drift > 2.0 && m_state == Imaging) {
            log(QString("Temperature drift %.1f%1C (%.1f -> %.1f) - triggering refocus")
                .arg(QChar(0x00B0)).arg(m_lastFocusTemperature).arg(temp));
            m_lastFocusTemperature = temp;
            triggerRefocus();
        }
    } else {
        m_lastFocusTemperature = temp;
    }
}

void AutopilotController::onTaskControllerStatusUpdated()
{
    if (m_paused || m_state == Idle) return;

    const TelescopeData &data = m_dataProcessor->getData();
    const TaskControllerStatus &tc = data.taskController;

    // Detect imaging completion
    if (m_state == Imaging && tc.state == "IDLE" && m_framesThisTarget > 0) {
        log(QString("Imaging session complete - %1 frames").arg(m_framesThisTarget));
        selectBestTarget();  // Move to next target
        return;
    }

    // Detect focus completion
    if (m_state == Refocusing && tc.state == "IDLE") {
        log("Refocus complete");
        m_lastFocusTemperature = m_dataProcessor->getData().environment.ambientTemperature;
        startImaging();
        return;
    }

    // Detect errors
    if (tc.state == "ERROR" || tc.state == "FAILED") {
        handleError(QString("TaskController error: %1").arg(tc.stage));
    }
}

void AutopilotController::onTiffImageDownloaded(const QString &filePath,
                                                 const QByteArray &imageData,
                                                 double ra, double dec, double exposure)
{
    Q_UNUSED(filePath)
    Q_UNUSED(ra)
    Q_UNUSED(dec)

    if (m_state != Imaging && m_state != Analyzing) return;
    if (m_paused) return;

    // Feed frame to image analyzer
    m_imageAnalyzer.addFrame(imageData, exposure);
    m_framesThisTarget++;
    m_framesSinceExposureChange++;

    // Check quality after accumulating a few frames
    if (m_framesSinceExposureChange >= EXPOSURE_SETTLE_FRAMES) {
        checkQuality();
    }

    checkFocusNeeded();
}

// ---------------------------------------------------------------------------
// Quality and focus checks
// ---------------------------------------------------------------------------

void AutopilotController::checkQuality()
{
    ImageAnalyzer::FrameStats stats = m_imageAnalyzer.lastFrameStats();
    double bg = stats.medianBackground;

    // Check if exposure needs adjustment
    if (bg < BG_LOW || bg > BG_HIGH) {
        adjustExposure();
        return;
    }

    // Check if we should move to next target
    if (m_framesThisTarget >= m_targetFrameCount) {
        log(QString("Reached target frame count (%1), selecting next target")
            .arg(m_targetFrameCount));
        stopImaging();
        selectBestTarget();
        return;
    }

    // Re-evaluate target scores periodically
    if (m_framesThisTarget % 20 == 0) {
        updateTargetScores();
        emit targetScoresUpdated();

        // Check if current target is still the best choice
        if (!m_targetScores.isEmpty() && m_hasTarget) {
            const TargetScore &best = m_targetScores.first();
            if (best.object.id != m_currentTarget.id && best.score > 0) {
                // Find current target's score
                for (const TargetScore &ts : m_targetScores) {
                    if (ts.object.id == m_currentTarget.id) {
                        if (best.score - ts.score > 20) {
                            log(QString("Better target available: %1 (score %.0f vs %.0f)")
                                .arg(best.object.name).arg(best.score).arg(ts.score));
                            stopImaging();
                            selectBestTarget();
                            return;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void AutopilotController::checkFocusNeeded()
{
    // Check sharpness trend
    if (m_imageAnalyzer.isSharpnessDeclining(5) && m_state == Imaging) {
        log("Sharpness declining over 5 frames - triggering refocus");
        triggerRefocus();
    }
}

// ---------------------------------------------------------------------------
// Error recovery
// ---------------------------------------------------------------------------

void AutopilotController::handleError(const QString &reason)
{
    m_consecutiveErrors++;
    log(QString("Error (%1/%2): %3").arg(m_consecutiveErrors).arg(3).arg(reason));

    if (m_consecutiveErrors >= 3) {
        log("CRITICAL: 3 consecutive errors - stopping autopilot");
        stop();
        return;
    }

    setState(RecoveringError);
    m_recoveryTimer.start();
}

void AutopilotController::attemptRecovery()
{
    log("Attempting recovery...");

    // Re-initialize telescope
    m_backend->initializeTelescope();

    // After a delay, try selecting a new target
    QTimer::singleShot(5000, this, [this]() {
        if (m_state == RecoveringError) {
            selectBestTarget();
        }
    });
}
