#include "LocationEntryDialog.h"
#include "LocationManager.h"
#include <QMessageBox>
#include <QTimer>

LocationEntryDialog::LocationEntryDialog(QWidget *parent)
    : QDialog(parent)
    , m_latitude(0.0)
    , m_longitude(0.0)
    , m_altitude(0.0)
    , m_hasValidLocation(false)
    , m_waitingForGPS(false)
    , m_locationManager(nullptr)
{
    setWindowTitle("Enter Observatory Location");
    setupUI();
}

LocationEntryDialog::~LocationEntryDialog()
{
    if (m_locationManager) {
        m_locationManager->stopUpdates();
    }
}

void LocationEntryDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Info label
    QLabel* infoLabel = new QLabel(
        "<b>Observatory Location Required</b><br><br>"
        "Please enter your geographic coordinates for telescope initialization.<br>"
        "You can use GPS to auto-fill these values, or enter them manually.<br><br>"
        "<i>Accurate coordinates are essential for proper telescope alignment.</i>"
    );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { padding: 10px; background-color: #f0f0f0; border-radius: 5px; }");
    mainLayout->addWidget(infoLabel);
    
    // GPS section
    QGroupBox* gpsGroup = new QGroupBox("GPS Location (Optional)");
    QVBoxLayout* gpsLayout = new QVBoxLayout(gpsGroup);
    
    m_useGPSButton = new QPushButton("📍 Use GPS to Auto-Fill");
    m_useGPSButton->setToolTip("Request permission and use GPS to automatically fill in your current location");
    m_useGPSButton->setStyleSheet(
        "QPushButton { "
        "    padding: 10px; "
        "    background-color: #007AFF; "
        "    color: white; "
        "    font-weight: bold; "
        "    border-radius: 5px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #0051D5; "
        "} "
        "QPushButton:disabled { "
        "    background-color: #cccccc; "
        "}"
    );
    connect(m_useGPSButton, &QPushButton::clicked, this, &LocationEntryDialog::onUseGPSClicked);
    gpsLayout->addWidget(m_useGPSButton);
    
    m_gpsProgressBar = new QProgressBar();
    m_gpsProgressBar->setRange(0, 0); // Indeterminate
    m_gpsProgressBar->setVisible(false);
    gpsLayout->addWidget(m_gpsProgressBar);
    
    m_gpsStatusLabel = new QLabel();
    m_gpsStatusLabel->setWordWrap(true);
    m_gpsStatusLabel->setVisible(false);
    gpsLayout->addWidget(m_gpsStatusLabel);
    
    mainLayout->addWidget(gpsGroup);
    
    // Manual coordinate input group
    QGroupBox* coordGroup = new QGroupBox("Manual Coordinates");
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
    QGroupBox* quickGroup = new QGroupBox("Quick Locations (Examples)");
    QGridLayout* quickLayout = new QGridLayout(quickGroup);
    
    struct QuickLocation {
        QString name;
        double lat;
        double lon;
    };
    
    QList<QuickLocation> locations = {
        {"London, UK", 51.5074, -0.1278},
        {"New York, USA", 40.7128, -74.0060},
        {"Sydney, Australia", -33.8688, 151.2093},
        {"Tokyo, Japan", 35.6762, 139.6503},
        {"Paris, France", 48.8566, 2.3522},
        {"Berlin, Germany", 52.5200, 13.4050}
    };
    
    int row = 0, col = 0;
    for (const auto& loc : locations) {
        QPushButton* btn = new QPushButton(loc.name);
        btn->setProperty("lat", loc.lat);
        btn->setProperty("lon", loc.lon);
        connect(btn, &QPushButton::clicked, this, &LocationEntryDialog::onQuickLocationClicked);
        quickLayout->addWidget(btn, row, col);
        
        col++;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }
    
    mainLayout->addWidget(quickGroup);
    
    // Validation status
    m_validationLabel = new QLabel();
    m_validationLabel->setWordWrap(true);
    m_validationLabel->setAlignment(Qt::AlignCenter);
    m_validationLabel->setStyleSheet("QLabel { padding: 8px; border-radius: 5px; }");
    mainLayout->addWidget(m_validationLabel);
    
    // Button box
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_okButton = new QPushButton("Initialize Telescope");
    m_okButton->setDefault(true);
    m_okButton->setStyleSheet(
        "QPushButton { "
        "    padding: 10px 20px; "
        "    background-color: #34C759; "
        "    color: white; "
        "    font-weight: bold; "
        "    border-radius: 5px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #248A3D; "
        "} "
        "QPushButton:disabled { "
        "    background-color: #cccccc; "
        "}"
    );
    connect(m_okButton, &QPushButton::clicked, this, &LocationEntryDialog::onAccepted);
    buttonLayout->addWidget(m_okButton);
    
    QPushButton* cancelButton = new QPushButton("Cancel");
    cancelButton->setStyleSheet("QPushButton { padding: 10px 20px; }");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Initial validation
    updateValidationStatus();
    
    setMinimumWidth(600);
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
        m_hasValidLocation = true;
        accept();
    } else {
        QMessageBox::warning(this, "Invalid Coordinates",
                           "Please check that your coordinates are within valid ranges:\n\n"
                           "Latitude: -90° to +90°\n"
                           "Longitude: -180° to +180°\n\n"
                           "Or use the GPS button to automatically fill in your current location.");
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

void LocationEntryDialog::onUseGPSClicked()
{
    if (!m_locationManager) {
        m_locationManager = new LocationManager(this);
        connect(m_locationManager, &LocationManager::locationUpdated,
                this, &LocationEntryDialog::onGPSLocationReceived);
        connect(m_locationManager, &LocationManager::errorOccurred,
                this, &LocationEntryDialog::onGPSError);
    }
    
    if (!m_locationManager->isLocationAvailable()) {
        QMessageBox::warning(this, "GPS Not Available",
                           "GPS location services are not available on this device.\n\n"
                           "Please enter your coordinates manually.");
        return;
    }
    
    // Show progress
    m_waitingForGPS = true;
    m_gpsProgressBar->setVisible(true);
    m_gpsStatusLabel->setText("🛰️ Requesting GPS location...\n"
                              "This may take a few seconds.");
    m_gpsStatusLabel->setStyleSheet("QLabel { color: #007AFF; }");
    m_gpsStatusLabel->setVisible(true);
    m_useGPSButton->setEnabled(false);
    enableInputs(false);
    
    // Request single location update
    m_locationManager->requestCurrentLocation();
    
    // Set a timeout
    QTimer::singleShot(30000, this, [this]() {
        if (m_waitingForGPS) {
            onGPSError("GPS request timed out after 30 seconds");
        }
    });
}

void LocationEntryDialog::onGPSLocationReceived()
{
    if (!m_waitingForGPS || !m_locationManager) {
        return;
    }
    
    m_waitingForGPS = false;
    
    // Auto-fill the coordinates
    double lat = m_locationManager->latitude();
    double lon = m_locationManager->longitude();
    double alt = m_locationManager->altitude();
    
    m_latitudeSpinBox->setValue(lat);
    m_longitudeSpinBox->setValue(lon);
    if (alt > -500.0 && alt < 9000.0) {
        m_altitudeSpinBox->setValue(alt);
    }
    
    // Show success
    m_gpsProgressBar->setVisible(false);
    m_gpsStatusLabel->setText(QString("✅ GPS location received!\n"
                                     "Lat: %1°, Lon: %2°, Alt: %3m\n"
                                     "You can now initialize the telescope.")
                             .arg(lat, 0, 'f', 6)
                             .arg(lon, 0, 'f', 6)
                             .arg(alt, 0, 'f', 1));
    m_gpsStatusLabel->setStyleSheet("QLabel { color: #34C759; font-weight: bold; }");
    
    m_useGPSButton->setEnabled(true);
    m_useGPSButton->setText("✅ GPS Location Received");
    enableInputs(true);
    
    // Stop location updates
    m_locationManager->stopUpdates();
}

void LocationEntryDialog::onGPSError(const QString& error)
{
    if (!m_waitingForGPS) {
        return;
    }
    
    m_waitingForGPS = false;
    m_gpsProgressBar->setVisible(false);
    
    m_gpsStatusLabel->setText(QString("⚠️ GPS Error: %1\n\n"
                                     "Please enter your coordinates manually.").arg(error));
    m_gpsStatusLabel->setStyleSheet("QLabel { color: #FF3B30; }");
    
    m_useGPSButton->setEnabled(true);
    m_useGPSButton->setText("🔄 Try GPS Again");
    enableInputs(true);
    
    if (m_locationManager) {
        m_locationManager->stopUpdates();
    }
}

void LocationEntryDialog::updateValidationStatus()
{
    if (validateCoordinates()) {
        m_validationLabel->setText("✅ Coordinates are valid - Ready to initialize telescope");
        m_validationLabel->setStyleSheet("QLabel { background-color: #d4edda; color: #155724; "
                                        "padding: 8px; border-radius: 5px; font-weight: bold; }");
        m_okButton->setEnabled(true);
    } else {
        m_validationLabel->setText("⚠️ Please enter valid coordinates or use GPS");
        m_validationLabel->setStyleSheet("QLabel { background-color: #fff3cd; color: #856404; "
                                        "padding: 8px; border-radius: 5px; }");
        m_okButton->setEnabled(false);
    }
}

bool LocationEntryDialog::validateCoordinates()
{
    double lat = m_latitudeSpinBox->value();
    double lon = m_longitudeSpinBox->value();
    
    // Check if coordinates are at default (0,0) - likely not set
    if (lat == 0.0 && lon == 0.0) {
        return false;
    }
    
    return (lat >= -90.0 && lat <= 90.0 && 
            lon >= -180.0 && lon <= 180.0);
}

void LocationEntryDialog::enableInputs(bool enabled)
{
    m_latitudeSpinBox->setEnabled(enabled);
    m_longitudeSpinBox->setEnabled(enabled);
    m_altitudeSpinBox->setEnabled(enabled);
}
