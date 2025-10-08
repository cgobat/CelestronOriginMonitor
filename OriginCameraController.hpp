// OriginCameraController.hpp
// Complete camera control interface for Celestron Origin telescope
#pragma once

#include <QObject>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "OriginBackend.hpp"

class OriginCameraController : public QObject
{
    Q_OBJECT

public:
    enum CameraMode {
        ManualMode,
        AutoMode
    };

    explicit OriginCameraController(OriginBackend* backend,
                                    const QString& telescopeIP,
                                    QObject *parent = nullptr);
    ~OriginCameraController();

    // Mode Control
    void setManualMode();
    void setAutoMode();
    void getCameraMode();
    
    // Exposure Settings
    void setExposure(double seconds);
    void setISO(int iso);
    void setCaptureParameters(double exposure, int iso);
    void getCaptureParameters();
    
    // Snapshot Control
    void takeSingleSnapshot();
    void takeSnapshot(double exposure, int iso);
    void downloadSnapshot(const QString& fileLocation, 
                         const QString& savePath);
    
    // Camera Info
    void getCameraInfo();
    
    // Current settings
    double currentExposure() const { return m_currentExposure; }
    int currentISO() const { return m_currentISO; }
    bool isManualMode() const { return m_isManual; }

signals:
    void modeChanged(bool isManual);
    void exposureChanged(double exposure);
    void isoChanged(int iso);
    void captureParametersChanged(double exposure, int iso);
    
    void snapshotReady(const QString& fileLocation, double ra, double dec);
    void snapshotDownloaded(const QString& localPath);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    
    void cameraInfoReceived(const QString& cameraID, const QString& model);
    void errorOccurred(const QString& error);

private slots:
    void handleWebSocketMessage(const QJsonObject& message);
    void handleDownloadFinished(QNetworkReply* reply);
    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    void sendCommand(const QJsonObject& command);
    void processNewImageReady(const QJsonObject& message);
    void processCaptureParametersResponse(const QJsonObject& message);
    void processModeResponse(const QJsonObject& message);
    void processCameraInfoResponse(const QJsonObject& message);
    
    OriginBackend* originBackend;
    QNetworkAccessManager* m_networkManager;
    QString m_telescopeIP;
    
    int m_sequenceId;
    
    // Current camera state
    bool m_isManual;
    double m_currentExposure;
    int m_currentISO;
    int m_currentBinning;
    int m_currentBitDepth;
    
    // Download tracking
    QString m_currentDownloadPath;
    QNetworkReply* m_currentDownload;
};
