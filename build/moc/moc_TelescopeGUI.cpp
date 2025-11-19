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
        "showLogReplay",
        "",
        "updateTaskControllerDisplay",
        "startUpButton",
        "startDownButton",
        "startLeftButton",
        "startRightButton",
        "slew",
        "cancelSlew",
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
        "startSlewAndImage",
        "cancelSlewAndImage",
        "slewAndImageTimerTimeout",
        "updateSlewAndImageStatus",
        "initializeTelescope",
        "startTelescopeAlignment",
        "checkMountStatus",
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
        "onSnapshotDownloadProgress",
        "bytesReceived",
        "bytesTotal"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'showLogReplay'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateTaskControllerDisplay'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startUpButton'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startDownButton'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startLeftButton'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startRightButton'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'slew'
        QtMocHelpers::SlotData<void(int, int)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 2 }, { QMetaType::Int, 2 },
        }}),
        // Slot 'cancelSlew'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTrackingError'
        QtMocHelpers::SlotData<void(const QString &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'onSlewCancel'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startDiscovery'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'stopDiscovery'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'processPendingDatagrams'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'connectToSelectedTelescope'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWebSocketConnected'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWebSocketDisconnected'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTextMessageReceived'
        QtMocHelpers::SlotData<void(const QString &)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 20 },
        }}),
        // Slot 'updateMountDisplay'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateCameraDisplay'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateFocuserDisplay'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateEnvironmentDisplay'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateImageDisplay'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateDiskDisplay'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateDewHeaterDisplay'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateTimeDisplay'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startSlewAndImage'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'cancelSlewAndImage'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'slewAndImageTimerTimeout'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateSlewAndImageStatus'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'initializeTelescope'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'startTelescopeAlignment'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'checkMountStatus'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCameraModeChanged'
        QtMocHelpers::SlotData<void(bool)>(36, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 37 },
        }}),
        // Slot 'onCaptureParametersChanged'
        QtMocHelpers::SlotData<void(double, int)>(38, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 39 }, { QMetaType::Int, 40 },
        }}),
        // Slot 'onSnapshotReady'
        QtMocHelpers::SlotData<void(const QString &, double, double)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 42 }, { QMetaType::Double, 43 }, { QMetaType::Double, 44 },
        }}),
        // Slot 'onSnapshotDownloaded'
        QtMocHelpers::SlotData<void(const QString &)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 46 },
        }}),
        // Slot 'onSnapshotDownloadProgress'
        QtMocHelpers::SlotData<void(qint64, qint64)>(47, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 48 }, { QMetaType::LongLong, 49 },
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
        case 0: _t->showLogReplay(); break;
        case 1: _t->updateTaskControllerDisplay(); break;
        case 2: _t->startUpButton(); break;
        case 3: _t->startDownButton(); break;
        case 4: _t->startLeftButton(); break;
        case 5: _t->startRightButton(); break;
        case 6: _t->slew((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->cancelSlew(); break;
        case 8: _t->onTrackingError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->onSlewCancel(); break;
        case 10: _t->startDiscovery(); break;
        case 11: _t->stopDiscovery(); break;
        case 12: _t->processPendingDatagrams(); break;
        case 13: _t->connectToSelectedTelescope(); break;
        case 14: _t->onWebSocketConnected(); break;
        case 15: _t->onWebSocketDisconnected(); break;
        case 16: _t->onTextMessageReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->updateMountDisplay(); break;
        case 18: _t->updateCameraDisplay(); break;
        case 19: _t->updateFocuserDisplay(); break;
        case 20: _t->updateEnvironmentDisplay(); break;
        case 21: _t->updateImageDisplay(); break;
        case 22: _t->updateDiskDisplay(); break;
        case 23: _t->updateDewHeaterDisplay(); break;
        case 24: _t->updateTimeDisplay(); break;
        case 25: _t->startSlewAndImage(); break;
        case 26: _t->cancelSlewAndImage(); break;
        case 27: _t->slewAndImageTimerTimeout(); break;
        case 28: _t->updateSlewAndImageStatus(); break;
        case 29: _t->initializeTelescope(); break;
        case 30: _t->startTelescopeAlignment(); break;
        case 31: _t->checkMountStatus(); break;
        case 32: _t->onCameraModeChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 33: _t->onCaptureParametersChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 34: _t->onSnapshotReady((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 35: _t->onSnapshotDownloaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 36: _t->onSnapshotDownloadProgress((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
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
        if (_id < 37)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 37;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 37)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 37;
    }
    return _id;
}
QT_WARNING_POP
