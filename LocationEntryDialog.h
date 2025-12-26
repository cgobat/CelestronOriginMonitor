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

class LocationEntryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LocationEntryDialog(QWidget *parent = nullptr);
    
    // Getters
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    double altitude() const { return m_altitude; }
    
    // Setters (for pre-filling)
    void setLatitude(double lat);
    void setLongitude(double lon);
    void setAltitude(double alt);
    
private slots:
    void onAccepted();
    void onQuickLocationClicked();
    void updateValidationStatus();
    
private:
    void setupUI();
    bool validateCoordinates();
    
    QDoubleSpinBox* m_latitudeSpinBox;
    QDoubleSpinBox* m_longitudeSpinBox;
    QDoubleSpinBox* m_altitudeSpinBox;
    QLabel* m_validationLabel;
    QPushButton* m_okButton;
    
    double m_latitude;
    double m_longitude;
    double m_altitude;
};

#endif // LOCATIONENTRYDIALOG_H
