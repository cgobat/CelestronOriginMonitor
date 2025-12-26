#ifndef LOCATIONMANAGER_H
#define LOCATIONMANAGER_H

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>

// Qt Positioning-based LocationManager
// Cross-platform: iOS, Android, Linux, etc.
// No Objective-C, no platform-specific code!

class LocationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double latitude READ latitude NOTIFY locationUpdated)
    Q_PROPERTY(double longitude READ longitude NOTIFY locationUpdated)
    Q_PROPERTY(double altitude READ altitude NOTIFY locationUpdated)
    Q_PROPERTY(bool hasLocation READ hasLocation NOTIFY locationUpdated)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorOccurred)

public:
    explicit LocationManager(QObject *parent = nullptr);
    ~LocationManager();

    // Request location updates
    Q_INVOKABLE void startUpdates();
    Q_INVOKABLE void stopUpdates();
    Q_INVOKABLE void requestCurrentLocation();
    
    // Getters
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    double altitude() const { return m_altitude; }
    bool hasLocation() const { return m_hasLocation; }
    QString errorMessage() const { return m_errorMessage; }
    
    // Check if location services are available
    Q_INVOKABLE bool isLocationAvailable() const;
    
signals:
    void locationUpdated();
    void errorOccurred(const QString &error);
    
private slots:
    void onPositionUpdated(const QGeoPositionInfo &info);
    void onError(QGeoPositionInfoSource::Error error);
    void onTimeout();
    
private:
    double m_latitude;
    double m_longitude;
    double m_altitude;
    bool m_hasLocation;
    QString m_errorMessage;
    
    QGeoPositionInfoSource *m_positionSource;
};

#endif // LOCATIONMANAGER_H
