/****************************************************************************
** Meta object code from reading C++ file 'TelescopeGUI.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../TelescopeGUI.hpp"
#include <QtGui/qtextcursor.h>
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TelescopeGUI.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12TelescopeGUIE_t {};
} // unnamed namespace

template <> constexpr inline auto TelescopeGUI::qt_create_metaobjectdata<qt_meta_tag_ZN12TelescopeGUIE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TelescopeGUI",
        "loadCometKernels",
        "",
        "startUpButton",
        "startDownButton",
        "startLeftButton",
        "startRightButton",
        "slew",
        "cancelSlew",
        "startCometTracking",
        "stopCometTracking",
        "onCometPositionUpdated",
        "SkyPosition",
        "pos",
        "onTrackingError",
        "error",
        "onSlewCancel",
        "startDiscovery",
        "stopDiscovery",
        "processPendingDatagrams",
        "connectToSelectedTelescope",
        "onWebSocketConnected",
        "onWebSocketDisconnected",
        "onTextMessageReceived",
        "message",
        "updateMountDisplay",
        "updateCameraDisplay",
        "updateFocuserDisplay",
        "updateEnvironmentDisplay",
        "updateImageDisplay",
        "updateDiskDisplay",
        "updateDewHeaterDisplay",
        "updateTimeDisplay",
        "startAutomaticDownload",
        "stopAutomaticDownload",
        "updateDownloadProgress",
        "currentFile",
        "filesCompleted",
        "totalFiles",
        "bytesReceived",
        "bytesTotal",
        "onDirectoryDownloadStarted",
        "directory",
        "onFileDownloadStarted",
        "fileName",
        "onFileDownloaded",
        "success",
        "onDirectoryDownloaded",
        "onAllDownloadsComplete",
        "startSlewAndImage",
        "cancelSlewAndImage",
        "slewAndImageTimerTimeout",
        "updateSlewAndImageStatus",
        "initializeTelescope",
        "startTelescopeAlignment",
        "checkMountStatus",
        "startAlpacaServer",
        "stopAlpacaServer",
        "onAlpacaServerStarted",
        "onAlpacaServerStopped",
        "onAlpacaRequestReceived",
        "method",
        "path",
        "clearAlpacaLog",
        "saveAlpacaLog",
        "onCameraModeChanged",
        "isManual",
        "onCaptureParametersChanged",
        "exposure",
        "iso",
        "onSnapshotReady",
        "fileLocation",
        "ra",
        "dec",
        "onSnapshotDownloaded",
        "localPath",
        "onSnapshotDownloadProgress"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'loadCometKernels'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startUpButton'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startDownButton'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startLeftButton'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startRightButton'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'slew'
        QtMocHelpers::SlotData<void(int, int)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 2 }, { QMetaType::Int, 2 },
        }}),
        // Slot 'cancelSlew'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startCometTracking'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'stopCometTracking'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCometPositionUpdated'
        QtMocHelpers::SlotData<void(const SkyPosition &)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Slot 'onTrackingError'
        QtMocHelpers::SlotData<void(const QString &)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Slot 'onSlewCancel'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startDiscovery'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'stopDiscovery'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'processPendingDatagrams'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'connectToSelectedTelescope'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWebSocketConnected'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWebSocketDisconnected'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTextMessageReceived'
        QtMocHelpers::SlotData<void(const QString &)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 24 },
        }}),
        // Slot 'updateMountDisplay'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateCameraDisplay'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateFocuserDisplay'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateEnvironmentDisplay'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateImageDisplay'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateDiskDisplay'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateDewHeaterDisplay'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateTimeDisplay'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startAutomaticDownload'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'stopAutomaticDownload'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateDownloadProgress'
        QtMocHelpers::SlotData<void(const QString &, int, int, qint64, qint64)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 36 }, { QMetaType::Int, 37 }, { QMetaType::Int, 38 }, { QMetaType::LongLong, 39 },
            { QMetaType::LongLong, 40 },
        }}),
        // Slot 'onDirectoryDownloadStarted'
        QtMocHelpers::SlotData<void(const QString &)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 42 },
        }}),
        // Slot 'onFileDownloadStarted'
        QtMocHelpers::SlotData<void(const QString &)>(43, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 44 },
        }}),
        // Slot 'onFileDownloaded'
        QtMocHelpers::SlotData<void(const QString &, bool)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 44 }, { QMetaType::Bool, 46 },
        }}),
        // Slot 'onDirectoryDownloaded'
        QtMocHelpers::SlotData<void(const QString &)>(47, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 42 },
        }}),
        // Slot 'onAllDownloadsComplete'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startSlewAndImage'
        QtMocHelpers::SlotData<void()>(49, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'cancelSlewAndImage'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'slewAndImageTimerTimeout'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateSlewAndImageStatus'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'initializeTelescope'
        QtMocHelpers::SlotData<void()>(53, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startTelescopeAlignment'
        QtMocHelpers::SlotData<void()>(54, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'checkMountStatus'
        QtMocHelpers::SlotData<void()>(55, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startAlpacaServer'
        QtMocHelpers::SlotData<void()>(56, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'stopAlpacaServer'
        QtMocHelpers::SlotData<void()>(57, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAlpacaServerStarted'
        QtMocHelpers::SlotData<void()>(58, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAlpacaServerStopped'
        QtMocHelpers::SlotData<void()>(59, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAlpacaRequestReceived'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(60, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 61 }, { QMetaType::QString, 62 },
        }}),
        // Slot 'clearAlpacaLog'
        QtMocHelpers::SlotData<void()>(63, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'saveAlpacaLog'
        QtMocHelpers::SlotData<void()>(64, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCameraModeChanged'
        QtMocHelpers::SlotData<void(bool)>(65, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 66 },
        }}),
        // Slot 'onCaptureParametersChanged'
        QtMocHelpers::SlotData<void(double, int)>(67, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 68 }, { QMetaType::Int, 69 },
        }}),
        // Slot 'onSnapshotReady'
        QtMocHelpers::SlotData<void(const QString &, double, double)>(70, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 71 }, { QMetaType::Double, 72 }, { QMetaType::Double, 73 },
        }}),
        // Slot 'onSnapshotDownloaded'
        QtMocHelpers::SlotData<void(const QString &)>(74, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 75 },
        }}),
        // Slot 'onSnapshotDownloadProgress'
        QtMocHelpers::SlotData<void(qint64, qint64)>(76, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 39 }, { QMetaType::LongLong, 40 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TelescopeGUI, qt_meta_tag_ZN12TelescopeGUIE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TelescopeGUI::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12TelescopeGUIE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12TelescopeGUIE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12TelescopeGUIE_t>.metaTypes,
    nullptr
} };

void TelescopeGUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TelescopeGUI *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->loadCometKernels(); break;
        case 1: _t->startUpButton(); break;
        case 2: _t->startDownButton(); break;
        case 3: _t->startLeftButton(); break;
        case 4: _t->startRightButton(); break;
        case 5: _t->slew((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 6: _t->cancelSlew(); break;
        case 7: _t->startCometTracking(); break;
        case 8: _t->stopCometTracking(); break;
        case 9: _t->onCometPositionUpdated((*reinterpret_cast< std::add_pointer_t<SkyPosition>>(_a[1]))); break;
        case 10: _t->onTrackingError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onSlewCancel(); break;
        case 12: _t->startDiscovery(); break;
        case 13: _t->stopDiscovery(); break;
        case 14: _t->processPendingDatagrams(); break;
        case 15: _t->connectToSelectedTelescope(); break;
        case 16: _t->onWebSocketConnected(); break;
        case 17: _t->onWebSocketDisconnected(); break;
        case 18: _t->onTextMessageReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: _t->updateMountDisplay(); break;
        case 20: _t->updateCameraDisplay(); break;
        case 21: _t->updateFocuserDisplay(); break;
        case 22: _t->updateEnvironmentDisplay(); break;
        case 23: _t->updateImageDisplay(); break;
        case 24: _t->updateDiskDisplay(); break;
        case 25: _t->updateDewHeaterDisplay(); break;
        case 26: _t->updateTimeDisplay(); break;
        case 27: _t->startAutomaticDownload(); break;
        case 28: _t->stopAutomaticDownload(); break;
        case 29: _t->updateDownloadProgress((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[5]))); break;
        case 30: _t->onDirectoryDownloadStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 31: _t->onFileDownloadStarted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 32: _t->onFileDownloaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 33: _t->onDirectoryDownloaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 34: _t->onAllDownloadsComplete(); break;
        case 35: _t->startSlewAndImage(); break;
        case 36: _t->cancelSlewAndImage(); break;
        case 37: _t->slewAndImageTimerTimeout(); break;
        case 38: _t->updateSlewAndImageStatus(); break;
        case 39: _t->initializeTelescope(); break;
        case 40: _t->startTelescopeAlignment(); break;
        case 41: _t->checkMountStatus(); break;
        case 42: _t->startAlpacaServer(); break;
        case 43: _t->stopAlpacaServer(); break;
        case 44: _t->onAlpacaServerStarted(); break;
        case 45: _t->onAlpacaServerStopped(); break;
        case 46: _t->onAlpacaRequestReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 47: _t->clearAlpacaLog(); break;
        case 48: _t->saveAlpacaLog(); break;
        case 49: _t->onCameraModeChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 50: _t->onCaptureParametersChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 51: _t->onSnapshotReady((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 52: _t->onSnapshotDownloaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 53: _t->onSnapshotDownloadProgress((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *TelescopeGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TelescopeGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12TelescopeGUIE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int TelescopeGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 54)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 54;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 54)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 54;
    }
    return _id;
}
QT_WARNING_POP
