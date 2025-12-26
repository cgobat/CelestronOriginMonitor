#include "LocationEntryDialog.h"
#include <QMessageBox>

LocationEntryDialog::LocationEntryDialog(QWidget *parent)
    : QDialog(parent)
    , m_latitude(0.0)
    , m_longitude(0.0)
    , m_altitude(0.0)
{
    setWindowTitle("Enter Location Manually");
    setupUI();
}

void LocationEntryDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Info label
    QLabel* infoLabel = new QLabel(
        "Enter your geographic coordinates for telescope initialization.\n"
        "You can find your coordinates using Google Maps or a GPS device."
    );
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);
    
    // Coordinate input group
    QGroupBox* coordGroup = new QGroupBox("Coordinates");
    QFormLayout* formLayout = new QFormLayout(coordGroup);
    
    // Latitude
    m_latitudeSpinBox = new QDoubleSpinBox();
    m_latitudeSpinBox->setRange(-90.0, 90.0);
    m_latitudeSpinBox->setDecimals(6);
    m_latitudeSpinBox->setSuffix("°");
    m_latitudeSpinBox->setToolTip("Latitude: -90° (South Pole) to +90° (North Pole)");
    connect(m_latitudeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &LocationEntryDialog::updateValidationStatus);
    formLayout->addRow("Latitude:", m_latitudeSpinBox);
    
    // Longitude
    m_longitudeSpinBox = new QDoubleSpinBox();
    m_longitudeSpinBox->setRange(-180.0, 180.0);
    m_longitudeSpinBox->setDecimals(6);
    m_longitudeSpinBox->setSuffix("°");
    m_longitudeSpinBox->setToolTip("Longitude: -180° (West) to +180° (East)");
    connect(m_longitudeSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &LocationEntryDialog::updateValidationStatus);
    formLayout->addRow("Longitude:", m_longitudeSpinBox);
    
    // Altitude (optional)
    m_altitudeSpinBox = new QDoubleSpinBox();
    m_altitudeSpinBox->setRange(-500.0, 9000.0);
    m_altitudeSpinBox->setDecimals(1);
    m_altitudeSpinBox->setSuffix(" m");
    m_altitudeSpinBox->setToolTip("Altitude above sea level (optional, leave as 0 if unknown)");
    formLayout->addRow("Altitude:", m_altitudeSpinBox);
    
    mainLayout->addWidget(coordGroup);
    
    // Quick location buttons
    QGroupBox* quickGroup = new QGroupBox("Quick Locations");
    QHBoxLayout* quickLayout = new QHBoxLayout(quickGroup);
    
    QPushButton* londonBtn = new QPushButton("London");
    londonBtn->setProperty("lat", 51.5074);
    londonBtn->setProperty("lon", -0.1278);
    connect(londonBtn, &QPushButton::clicked, this, &LocationEntryDialog::onQuickLocationClicked);
    quickLayout->addWidget(londonBtn);
    
    QPushButton* nyBtn = new QPushButton("New York");
    nyBtn->setProperty("lat", 40.7128);
    nyBtn->setProperty("lon", -74.0060);
    connect(nyBtn, &QPushButton::clicked, this, &LocationEntryDialog::onQuickLocationClicked);
    quickLayout->addWidget(nyBtn);
    
    QPushButton* sydneyBtn = new QPushButton("Sydney");
    sydneyBtn->setProperty("lat", -33.8688);
    sydneyBtn->setProperty("lon", 151.2093);
    connect(sydneyBtn, &QPushButton::clicked, this, &LocationEntryDialog::onQuickLocationClicked);
    quickLayout->addWidget(sydneyBtn);
    
    QPushButton* tokyoBtn = new QPushButton("Tokyo");
    tokyoBtn->setProperty("lat", 35.6762);
    tokyoBtn->setProperty("lon", 139.6503);
    connect(tokyoBtn, &QPushButton::clicked, this, &LocationEntryDialog::onQuickLocationClicked);
    quickLayout->addWidget(tokyoBtn);
    
    mainLayout->addWidget(quickGroup);
    
    // Validation status
    m_validationLabel = new QLabel();
    m_validationLabel->setWordWrap(true);
    mainLayout->addWidget(m_validationLabel);
    
    // Button box
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_okButton = new QPushButton("OK");
    m_okButton->setDefault(true);
    connect(m_okButton, &QPushButton::clicked, this, &LocationEntryDialog::onAccepted);
    buttonLayout->addWidget(m_okButton);
    
    QPushButton* cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Initial validation
    updateValidationStatus();
    
    setMinimumWidth(500);
}

void LocationEntryDialog::setLatitude(double lat)
{
    m_latitudeSpinBox->setValue(lat);
}

void LocationEntryDialog::setLongitude(double lon)
{
    m_longitudeSpinBox->setValue(lon);
}

void LocationEntryDialog::setAltitude(double alt)
{
    m_altitudeSpinBox->setValue(alt);
}

void LocationEntryDialog::onAccepted()
{
    if (validateCoordinates()) {
        m_latitude = m_latitudeSpinBox->value();
        m_longitude = m_longitudeSpinBox->value();
        m_altitude = m_altitudeSpinBox->value();
        accept();
    } else {
        QMessageBox::warning(this, "Invalid Coordinates",
                           "Please check that your coordinates are within valid ranges:\n"
                           "Latitude: -90° to +90°\n"
                           "Longitude: -180° to +180°");
    }
}

void LocationEntryDialog::onQuickLocationClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        double lat = btn->property("lat").toDouble();
        double lon = btn->property("lon").toDouble();
        m_latitudeSpinBox->setValue(lat);
        m_longitudeSpinBox->setValue(lon);
    }
}

void LocationEntryDialog::updateValidationStatus()
{
    if (validateCoordinates()) {
        m_validationLabel->setText("✓ Coordinates are valid");
        m_validationLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
        m_okButton->setEnabled(true);
    } else {
        m_validationLabel->setText("⚠ Please enter valid coordinates");
        m_validationLabel->setStyleSheet("QLabel { color: orange; font-weight: bold; }");
        m_okButton->setEnabled(false);
    }
}

bool LocationEntryDialog::validateCoordinates()
{
    double lat = m_latitudeSpinBox->value();
    double lon = m_longitudeSpinBox->value();
    
    return (lat >= -90.0 && lat <= 90.0 && 
            lon >= -180.0 && lon <= 180.0);
}
