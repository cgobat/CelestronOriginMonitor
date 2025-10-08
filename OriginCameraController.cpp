// OriginCameraController.cpp
#include "OriginCameraController.hpp"
#include <QDebug>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QNetworkRequest>
#include <QUrl>

OriginCameraController::OriginCameraController(OriginBackend* backend,
                                               const QString& telescopeIP,
                                               QObject *parent)
    : QObject(parent)
    , m_backend(backend)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_telescopeIP(telescopeIP)
    , m_sequenceId(20000)
    , m_isManual(false)
    , m_currentExposure(0.1)
    , m_currentISO(200)
    , m_currentBinning(1)
    , m_currentBitDepth(24)
    , m_currentDownload(nullptr)
{
    // Connect to network manager signals
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &OriginCameraController::handleDownloadFinished);
    
    qDebug() << "OriginCameraController initialized for" << m_telescopeIP;
    
    // Get initial camera state
    getCameraMode();
    getCaptureParameters();
}

OriginCameraController::~OriginCameraController()
{
    if (m_currentDownload) {
        m_currentDownload->abort();
    }
}

// ============================================================================
// MODE CONTROL
// ============================================================================

void OriginCameraController::setManualMode()
{
    QJsonObject cmd;
    cmd["Command"] = "SetEnableManual";
    cmd["Destination"] = "LiveStream";
    cmd["SequenceID"] = m_sequenceId++;
    cmd["Source"] = "CometTracker";
    cmd["Type"] = "Command";
    
    sendCommand(cmd);
    qDebug() << "Setting manual mode";
}

void OriginCameraController::setAutoMode()
{
    QJsonObject cmd;
    cmd["Command"] = "SetEnableAuto";
    cmd["Destination"] = "LiveStream";
    cmd["SequenceID"] = m_sequenceId++;
    cmd["Source"] = "CometTracker";
    cmd["Type"] = "Command";
    
    sendCommand(cmd);
    qDebug() << "Setting auto mode";
}

void OriginCameraController::getCameraMode()
{
    QJsonObject cmd;
    cmd["Command"] = "GetEnableManual";
    cmd["Destination"] = "LiveStream";
    cmd["SequenceID"] = m_sequenceId++;
    cmd["Source"] = "CometTracker";
    cmd["Type"] = "Command";
    
    sendCommand(cmd);
}

// ============================================================================
// EXPOSURE SETTINGS
// ============================================================================

void OriginCameraController::setExposure(double seconds)
{
    setCaptureParameters(seconds, m_currentISO);
}

void OriginCameraController::setISO(int iso)
{
    setCaptureParameters(m_currentExposure, iso);
}

void OriginCameraController::setCaptureParameters(double exposure, int iso)
{
    QJsonObject cmd;
    cmd["Command"] = "SetCaptureParameters";
    cmd["Destination"] = "Camera";
    cmd["SequenceID"] = m_sequenceId++;
    cmd["Source"] = "CometTracker";
    cmd["Type"] = "Command";
    cmd["Exposure"] = exposure;
    cmd["ISO"] = iso;
    
    sendCommand(cmd);
    
    // Update local cache
    m_currentExposure = exposure;
    m_currentISO = iso;
    
    qDebug() << "Setting capture parameters: Exposure =" << exposure 
             << "ISO =" << iso;
    
    emit captureParametersChanged(exposure, iso);
}

void OriginCameraController::getCaptureParameters()
{
    QJsonObject cmd;
    cmd["Command"] = "GetCaptureParameters";
    cmd["Destination"] = "Camera";
    cmd["SequenceID"] = m_sequenceId++;
    cmd["Source"] = "CometTracker";
    cmd["Type"] = "Command";
    
    sendCommand(cmd);
}

// ============================================================================
// SNAPSHOT CONTROL
// ============================================================================

void OriginCameraController::takeSingleSnapshot()
{
    takeSnapshot(m_currentExposure, m_currentISO);
}

void OriginCameraController::takeSnapshot(double exposure, int iso)
{
    // RunSampleCapture command triggers a single snapshot
    QJsonObject cmd;
    cmd["Command"] = "RunSampleCapture";
    cmd["Destination"] = "TaskController";
    cmd["SequenceID"] = m_sequenceId++;
    cmd["Source"] = "CometTracker";
    cmd["Type"] = "Command";
    cmd["ExposureTime"] = exposure;
    cmd["ISO"] = iso;
    
    sendCommand(cmd);
    qDebug() << "Taking snapshot: Exposure =" << exposure << "ISO =" << iso;
}

void OriginCameraController::downloadSnapshot(const QString& fileLocation,
                                              const QString& savePath)
{
    // The Origin telescope serves images via HTTP
    // FileLocation format: "Images/Temp/1.jpg" or "Images/Temp/sample5.tiff"
    
    if (m_telescopeIP.isEmpty()) {
        emit errorOccurred("Telescope IP not set");
        return;
    }
    
    // Cancel any existing download
    if (m_currentDownload) {
        m_currentDownload->abort();
        m_currentDownload = nullptr;
    }
    
    // Construct URL: http://<telescope_ip>/<fileLocation>
    QString urlString = QString("http://%1/%2")
                        .arg(m_telescopeIP)
                        .arg(fileLocation);
    
    QUrl url(urlString);
    QNetworkRequest request(url);
    
    qDebug() << "Downloading snapshot from:" << urlString;
    qDebug() << "Saving to:" << savePath;
    
    m_currentDownloadPath = savePath;
    m_currentDownload = m_networkManager->get(request);
    
    // Track download progress
    connect(m_currentDownload, &QNetworkReply::downloadProgress,
            this, &OriginCameraController::handleDownloadProgress);
}

// ============================================================================
// CAMERA INFO
// ============================================================================

void OriginCameraController::getCameraInfo()
{
    QJsonObject cmd;
    cmd["Command"] = "GetCameraInfo";
    cmd["Destination"] = "Camera";
    cmd["SequenceID"] = m_sequenceId++;
    cmd["Source"] = "CometTracker";
    cmd["Type"] = "Command";
    
    sendCommand(cmd);
}

// ============================================================================
// PRIVATE METHODS
// ============================================================================

void OriginCameraController::sendCommand(const QJsonObject& command)
{
    if (!m_backend) {
        emit errorOccurred("Backend not available");
        return;
    }
    
    // Send via OriginBackend's WebSocket connection
    // Note: You need to add sendCommand() method to OriginBackend
    m_backend->sendCommand(command);
}

void OriginCameraController::handleWebSocketMessage(const QJsonObject& message)
{
    QString command = message["Command"].toString();
    QString type = message["Type"].toString();
    
    if (type == "Notification") {
        // Handle notifications
        if (command == "NewImageReady") {
            processNewImageReady(message);
        }
    } 
    else if (type == "Response") {
        // Handle command responses
        int errorCode = message["ErrorCode"].toInt();
        if (errorCode != 0) {
            QString errorMsg = message["ErrorMessage"].toString();
            qWarning() << "Command error:" << errorCode << errorMsg;
            emit errorOccurred(errorMsg);
            return;
        }
        
        if (command == "GetCaptureParameters") {
            processCaptureParametersResponse(message);
        }
        else if (command == "GetEnableManual" || 
                 command == "SetEnableManual" || 
                 command == "SetEnableAuto") {
            processModeResponse(message);
        }
        else if (command == "GetCameraInfo") {
            processCameraInfoResponse(message);
        }
    }
}

void OriginCameraController::processNewImageReady(const QJsonObject& message)
{
    QString fileLocation = message["FileLocation"].toString();
    double ra = message["Ra"].toDouble();
    double dec = message["Dec"].toDouble();
    double exposureTime = message["ExposureTime"].toDouble();
    
    qDebug() << "New image ready:" << fileLocation;
    qDebug() << "  Exposure:" << exposureTime << "s";
    qDebug() << "  Position: RA =" << ra << "Dec =" << dec;
    
    emit snapshotReady(fileLocation, ra, dec);
}

void OriginCameraController::processCaptureParametersResponse(const QJsonObject& message)
{
    m_currentExposure = message["Exposure"].toDouble();
    m_currentISO = message["ISO"].toInt();
    m_currentBinning = message["Binning"].toInt();
    m_currentBitDepth = message["BitDepth"].toInt();
    
    qDebug() << "Capture parameters:";
    qDebug() << "  Exposure:" << m_currentExposure;
    qDebug() << "  ISO:" << m_currentISO;
    qDebug() << "  Binning:" << m_currentBinning;
    qDebug() << "  BitDepth:" << m_currentBitDepth;
    
    emit captureParametersChanged(m_currentExposure, m_currentISO);
}

void OriginCameraController::processModeResponse(const QJsonObject& message)
{
    if (message.contains("IsManual")) {
        m_isManual = message["IsManual"].toBool();
        qDebug() << "Camera mode:" << (m_isManual ? "Manual" : "Auto");
        emit modeChanged(m_isManual);
    }
}

void OriginCameraController::processCameraInfoResponse(const QJsonObject& message)
{
    QString cameraID = message["CameraID"].toString();
    QString cameraModel = message["CameraModel"].toString();
    int defaultISO = message["DefaultIso"].toInt();
    
    qDebug() << "Camera Info:";
    qDebug() << "  ID:" << cameraID;
    qDebug() << "  Model:" << cameraModel;
    qDebug() << "  Default ISO:" << defaultISO;
    
    emit cameraInfoReceived(cameraID, cameraModel);
}

void OriginCameraController::handleDownloadFinished(QNetworkReply* reply)
{
    if (reply != m_currentDownload) {
        return; // Not our download
    }
    
    m_currentDownload = nullptr;
    
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Download error:" << reply->errorString();
        emit errorOccurred("Download failed: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // Read the downloaded data
    QByteArray imageData = reply->readAll();
    
    // Ensure directory exists
    QFileInfo fileInfo(m_currentDownloadPath);
    QDir().mkpath(fileInfo.absolutePath());
    
    // Save to file
    QFile file(m_currentDownloadPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << m_currentDownloadPath;
        emit errorOccurred("Failed to save snapshot");
        reply->deleteLater();
        return;
    }
    
    file.write(imageData);
    file.close();
    
    qDebug() << "Snapshot saved to:" << m_currentDownloadPath;
    qDebug() << "File size:" << imageData.size() << "bytes";
    
    emit snapshotDownloaded(m_currentDownloadPath);
    
    reply->deleteLater();
}

void OriginCameraController::handleDownloadProgress(qint64 bytesReceived, 
                                                    qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int percent = (bytesReceived * 100) / bytesTotal;
        qDebug() << "Download progress:" << percent << "%"
                 << "(" << bytesReceived << "/" << bytesTotal << "bytes)";
    }
    emit downloadProgress(bytesReceived, bytesTotal);
}
