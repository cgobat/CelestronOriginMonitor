#include "LocationManager.h"
#include <QDebug>
#include <QTimer>

LocationManager::LocationManager(QObject *parent)
    : QObject(parent)
    , m_latitude(0.0)
    , m_longitude(0.0)
    , m_altitude(0.0)
    , m_hasLocation(false)
    , m_positionSource(nullptr)
{
    // Create position source (Qt automatically picks best available: GPS, network, etc.)
    m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);
    
    if (m_positionSource) {
        qDebug() << "Location source available:" << m_positionSource->sourceName();
        
        // Configure for best accuracy
        m_positionSource->setPreferredPositioningMethods(
            QGeoPositionInfoSource::SatellitePositioningMethods |
            QGeoPositionInfoSource::NonSatellitePositioningMethods
        );
        
        // Set update interval (milliseconds)
        m_positionSource->setUpdateInterval(5000); // 5 seconds
        
        // Connect signals - Qt 6 compatible syntax
        connect(m_positionSource, &QGeoPositionInfoSource::positionUpdated,
                this, &LocationManager::onPositionUpdated);
        
        // Error signal - use simpler connection syntax for Qt 6
        connect(m_positionSource, &QGeoPositionInfoSource::errorOccurred,
                this, &LocationManager::onError);
        
        // Note: updateTimeout was removed in Qt 6
        // We'll handle timeouts in the request methods instead
        
    } else {
        qWarning() << "No location source available on this platform";
        m_errorMessage = "Location services not available on this device";
    }
}

LocationManager::~LocationManager()
{
    if (m_positionSource) {
        m_positionSource->stopUpdates();
    }
}

void LocationManager::startUpdates()
{
    if (!m_positionSource) {
        qWarning() << "Location source not available";
        emit errorOccurred("Location services not available");
        return;
    }
    
    qDebug() << "Starting location updates";
    m_positionSource->startUpdates();
}

void LocationManager::stopUpdates()
{
    if (m_positionSource) {
        qDebug() << "Stopping location updates";
        m_positionSource->stopUpdates();
    }
}

void LocationManager::requestCurrentLocation()
{
    if (!m_positionSource) {
        qWarning() << "Location source not available";
        emit errorOccurred("Location services not available");
        return;
    }
    
    qDebug() << "Requesting single location update";
    m_positionSource->requestUpdate(30000); // 30 second timeout
    
    // Since updateTimeout signal was removed in Qt 6, 
    // we'll use a QTimer to detect timeout
    QTimer::singleShot(31000, this, [this]() {
        if (!m_hasLocation) {
            onTimeout();
        }
    });
}

bool LocationManager::isLocationAvailable() const
{
    return m_positionSource != nullptr;
}

void LocationManager::onPositionUpdated(const QGeoPositionInfo &info)
{
    if (!info.isValid()) {
        qWarning() << "Received invalid position info";
        return;
    }
    
    QGeoCoordinate coord = info.coordinate();
    
    m_latitude = coord.latitude();
    m_longitude = coord.longitude();
    m_altitude = coord.altitude();
    m_hasLocation = true;
    
    qDebug() << "Location updated:"
             << "Lat:" << m_latitude
             << "Lon:" << m_longitude
             << "Alt:" << m_altitude << "m"
             << "Accuracy:" << info.attribute(QGeoPositionInfo::HorizontalAccuracy) << "m";
    
    emit locationUpdated();
}

void LocationManager::onError(QGeoPositionInfoSource::Error error)
{
    QString errorMsg;
    
    switch (error) {
        case QGeoPositionInfoSource::AccessError:
            errorMsg = "Location access denied. Please enable location services in system settings.";
            break;
        case QGeoPositionInfoSource::ClosedError:
            errorMsg = "Location service connection closed.";
            break;
        case QGeoPositionInfoSource::UnknownSourceError:
            errorMsg = "Unknown location service error.";
            break;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        case QGeoPositionInfoSource::NoError:
            // No error, shouldn't happen
            return;
#endif
        default:
            errorMsg = "Location service error occurred.";
    }
    
    qWarning() << "Location error:" << errorMsg;
    m_errorMessage = errorMsg;
    emit errorOccurred(errorMsg);
}

void LocationManager::onTimeout()
{
    qWarning() << "Location request timed out";
    m_errorMessage = "Location request timed out. Please try again.";
    emit errorOccurred(m_errorMessage);
}
