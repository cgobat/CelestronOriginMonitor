/****************************************************************************
** Meta object code from reading C++ file 'OriginCameraController.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OriginCameraController.hpp"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OriginCameraController.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
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
struct qt_meta_tag_ZN22OriginCameraControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto OriginCameraController::qt_create_metaobjectdata<qt_meta_tag_ZN22OriginCameraControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "OriginCameraController",
        "modeChanged",
        "",
        "isManual",
        "exposureChanged",
        "exposure",
        "isoChanged",
        "iso",
        "captureParametersChanged",
        "snapshotReady",
        "fileLocation",
        "ra",
        "dec",
        "snapshotDownloaded",
        "localPath",
        "downloadProgress",
        "bytesReceived",
        "bytesTotal",
        "cameraInfoReceived",
        "cameraID",
        "model",
        "errorOccurred",
        "error",
        "handleWebSocketMessage",
        "message",
        "handleDownloadFinished",
        "QNetworkReply*",
        "reply",
        "handleDownloadProgress"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'modeChanged'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'exposureChanged'
        QtMocHelpers::SignalData<void(double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 5 },
        }}),
        // Signal 'isoChanged'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Signal 'captureParametersChanged'
        QtMocHelpers::SignalData<void(double, int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 5 }, { QMetaType::Int, 7 },
        }}),
        // Signal 'snapshotReady'
        QtMocHelpers::SignalData<void(const QString &, double, double)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::Double, 11 }, { QMetaType::Double, 12 },
        }}),
        // Signal 'snapshotDownloaded'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'downloadProgress'
        QtMocHelpers::SignalData<void(qint64, qint64)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::LongLong, 16 }, { QMetaType::LongLong, 17 },
        }}),
        // Signal 'cameraInfoReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 19 }, { QMetaType::QString, 20 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 22 },
        }}),
        // Slot 'handleWebSocketMessage'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QJsonObject, 24 },
        }}),
        // Slot 'handleDownloadFinished'
        QtMocHelpers::SlotData<void(QNetworkReply *)>(25, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 26, 27 },
        }}),
        // Slot 'handleDownloadProgress'
        QtMocHelpers::SlotData<void(qint64, qint64)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 16 }, { QMetaType::LongLong, 17 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<OriginCameraController, qt_meta_tag_ZN22OriginCameraControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject OriginCameraController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22OriginCameraControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22OriginCameraControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN22OriginCameraControllerE_t>.metaTypes,
    nullptr
} };

void OriginCameraController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OriginCameraController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->modeChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->exposureChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->isoChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->captureParametersChanged((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->snapshotReady((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 5: _t->snapshotDownloaded((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->downloadProgress((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        case 7: _t->cameraInfoReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->handleWebSocketMessage((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 10: _t->handleDownloadFinished((*reinterpret_cast< std::add_pointer_t<QNetworkReply*>>(_a[1]))); break;
        case 11: _t->handleDownloadProgress((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QNetworkReply* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(bool )>(_a, &OriginCameraController::modeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(double )>(_a, &OriginCameraController::exposureChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(int )>(_a, &OriginCameraController::isoChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(double , int )>(_a, &OriginCameraController::captureParametersChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(const QString & , double , double )>(_a, &OriginCameraController::snapshotReady, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(const QString & )>(_a, &OriginCameraController::snapshotDownloaded, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(qint64 , qint64 )>(_a, &OriginCameraController::downloadProgress, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(const QString & , const QString & )>(_a, &OriginCameraController::cameraInfoReceived, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginCameraController::*)(const QString & )>(_a, &OriginCameraController::errorOccurred, 8))
            return;
    }
}

const QMetaObject *OriginCameraController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OriginCameraController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN22OriginCameraControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int OriginCameraController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void OriginCameraController::modeChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void OriginCameraController::exposureChanged(double _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void OriginCameraController::isoChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void OriginCameraController::captureParametersChanged(double _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void OriginCameraController::snapshotReady(const QString & _t1, double _t2, double _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2, _t3);
}

// SIGNAL 5
void OriginCameraController::snapshotDownloaded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void OriginCameraController::downloadProgress(qint64 _t1, qint64 _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void OriginCameraController::cameraInfoReceived(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void OriginCameraController::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}
QT_WARNING_POP
