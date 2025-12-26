#ifndef LOCATIONENTRYDIALOG_H
#define LOCATIONENTRYDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QProgressBar>

// Forward declaration
class LocationManager;

class LocationEntryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LocationEntryDialog(QWidget *parent = nullptr);
    ~LocationEntryDialog();
    
    // Getters
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    double altitude() const { return m_altitude; }
    
    // Check if location is valid and ready
    bool hasValidLocation() const { return m_hasValidLocation; }
    
    // Setters (for pre-filling)
    void setLatitude(double lat);
    void setLongitude(double lon);
    void setAltitude(double alt);
    
private slots:
    void onAccepted();
    void onQuickLocationClicked();
    void onUseGPSClicked();
    void onGPSLocationReceived();
    void onGPSError(const QString& error);
    void updateValidationStatus();
    
private:
    void setupUI();
    bool validateCoordinates();
    void enableInputs(bool enabled);
    
    // UI elements
    QDoubleSpinBox* m_latitudeSpinBox;
    QDoubleSpinBox* m_longitudeSpinBox;
    QDoubleSpinBox* m_altitudeSpinBox;
    QLabel* m_validationLabel;
    QLabel* m_gpsStatusLabel;
    QPushButton* m_okButton;
    QPushButton* m_useGPSButton;
    QProgressBar* m_gpsProgressBar;
    
    // Location manager for GPS
    LocationManager* m_locationManager;
    
    // State
    double m_latitude;
    double m_longitude;
    double m_altitude;
    bool m_hasValidLocation;
    bool m_waitingForGPS;
};

#endif // LOCATIONENTRYDIALOG_H
