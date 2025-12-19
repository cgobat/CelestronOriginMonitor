// PlatformUtils.hpp
// Cross-platform utilities for iOS and macOS

#pragma once

#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

class PlatformUtils {
public:
    /**
     * @brief Get the appropriate documents directory for the platform
     * @return Path to documents directory
     */
    static QString getDocumentsPath() {
#ifdef Q_OS_IOS
        // iOS: Use app's Documents directory
        QStringList paths = QStandardPaths::standardLocations(
            QStandardPaths::DocumentsLocation);
        if (!paths.isEmpty()) {
            return paths.first();
        }
        return QDir::homePath();
#else
        // macOS/Desktop: Use app data location
        QString path = QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation);
        QDir dir;
        if (!dir.exists(path)) {
            dir.mkpath(path);
        }
        return path;
#endif
    }
    
    /**
     * @brief Get path to bundled resources
     * @param resourceName Name of resource file or directory
     * @return Full path to resource
     */
    static QString getResourcePath(const QString& resourceName = "") {
#ifdef Q_OS_IOS
        // iOS: Resources are in app bundle
        QString bundlePath = QCoreApplication::applicationDirPath();
        if (resourceName.isEmpty()) {
            return bundlePath;
        }
        return bundlePath + "/" + resourceName;
#else
        // macOS development: Use project directory
        // In production, this would also use the bundle
        if (resourceName.isEmpty()) {
            return QCoreApplication::applicationDirPath();
        }
        return QCoreApplication::applicationDirPath() + "/../Resources/" + resourceName;
#endif
    }
    
    /**
     * @brief Get path to SPICE kernel files
     * @return Path to kernel directory
     */
    static QString getKernelPath() {
#ifdef Q_OS_IOS
        return getResourcePath("spice_kernels");
#else
        // Development path for macOS
        return "/Users/jonathan/spice_kernels/";
#endif
    }
    
    /**
     * @brief Get cache directory for temporary files
     * @return Path to cache directory
     */
    static QString getCachePath() {
        QString path = QStandardPaths::writableLocation(
            QStandardPaths::CacheLocation);
        QDir dir;
        if (!dir.exists(path)) {
            dir.mkpath(path);
        }
        return path;
    }
    
    /**
     * @brief Get path for log files
     * @return Path to log directory
     */
    static QString getLogPath() {
#ifdef Q_OS_IOS
        // iOS: Logs go in Documents so they're accessible via Files app
        return getDocumentsPath() + "/Logs";
#else
        // macOS: Use standard log location
        return getDocumentsPath() + "/Logs";
#endif
    }
    
    /**
     * @brief Check if running on iOS
     * @return true if iOS, false otherwise
     */
    static bool isIOS() {
#ifdef Q_OS_IOS
        return true;
#else
        return false;
#endif
    }
    
    /**
     * @brief Check if running on macOS
     * @return true if macOS, false otherwise
     */
    static bool isMacOS() {
#ifdef Q_OS_MACOS
        return true;
#else
        return false;
#endif
    }
    
    /**
     * @brief Get recommended UI font size for platform
     * @return Font size in points
     */
    static int getRecommendedFontSize() {
#ifdef Q_OS_IOS
        return 16; // Larger for touch interfaces
#else
        return 12; // Standard desktop size
#endif
    }
    
    /**
     * @brief Get recommended minimum button height for platform
     * @return Height in pixels/points
     */
    static int getMinimumButtonHeight() {
#ifdef Q_OS_IOS
        return 44; // Apple Human Interface Guidelines
#else
        return 30; // Desktop standard
#endif
    }
    
    /**
     * @brief Get recommended spacing for UI elements
     * @return Spacing in pixels/points
     */
    static int getRecommendedSpacing() {
#ifdef Q_OS_IOS
        return 15; // More spacing for touch
#else
        return 8;  // Desktop standard
#endif
    }
    
    /**
     * @brief Create directory if it doesn't exist
     * @param path Directory path to create
     * @return true if directory exists or was created
     */
    static bool ensureDirectoryExists(const QString& path) {
        QDir dir;
        if (!dir.exists(path)) {
            return dir.mkpath(path);
        }
        return true;
    }
    
    /**
     * @brief Log a message to platform-appropriate location
     * @param message Message to log
     */
    static void logMessage(const QString& message) {
        qDebug() << message;
        
        // Also write to file
        QString logDir = getLogPath();
        ensureDirectoryExists(logDir);
        
        QString logFile = logDir + "/app.log";
        QFile file(logFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << QDateTime::currentDateTime().toString(Qt::ISODate) 
                << " " << message << "\n";
            file.close();
        }
    }
    
    /**
     * @brief Get available memory (for iOS memory warnings)
     * @return Available memory in MB (approximate)
     */
    static qint64 getAvailableMemory() {
#ifdef Q_OS_IOS
        // iOS-specific memory check would go here
        // For now, return a placeholder
        return -1; // Not implemented
#else
        return -1; // Not applicable on desktop
#endif
    }
};

/**
 * @brief iOS-specific UI configuration helper
 */
class iOSUIHelper {
public:
    /**
     * @brief Configure a button for iOS
     * @param button Button to configure
     */
    static void configureButton(QPushButton* button) {
#ifdef Q_OS_IOS
        if (!button) return;
        
        button->setMinimumHeight(PlatformUtils::getMinimumButtonHeight());
        
        QFont font = button->font();
        font.setPointSize(PlatformUtils::getRecommendedFontSize());
        button->setFont(font);
        
        // Add some padding for better touch targets
        button->setStyleSheet(
            "QPushButton { padding: 8px 16px; }"
        );
#endif
    }
    
    /**
     * @brief Configure a widget for iOS scrolling
     * @param widget Widget to wrap in scroll area
     * @return QScrollArea containing the widget (or original widget on desktop)
     */
    static QWidget* makeScrollable(QWidget* widget) {
#ifdef Q_OS_IOS
        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(widget);
        return scrollArea;
#else
        return widget; // No wrapping on desktop
#endif
    }
    
    /**
     * @brief Apply iOS-friendly styling to a label
     * @param label Label to style
     */
    static void configureLabel(QLabel* label) {
#ifdef Q_OS_IOS
        if (!label) return;
        
        QFont font = label->font();
        font.setPointSize(PlatformUtils::getRecommendedFontSize() - 2);
        label->setFont(font);
#endif
    }
};

// Convenience macros
#ifdef Q_OS_IOS
    #define IS_MOBILE true
    #define IS_DESKTOP false
#else
    #define IS_MOBILE false
    #define IS_DESKTOP true
#endif
