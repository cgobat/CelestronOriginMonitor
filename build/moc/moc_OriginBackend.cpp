/****************************************************************************
** Meta object code from reading C++ file 'OriginBackend.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../OriginBackend.hpp"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OriginBackend.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
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
struct qt_meta_tag_ZN13OriginBackendE_t {};
} // unnamed namespace

template <> constexpr inline auto OriginBackend::qt_create_metaobjectdata<qt_meta_tag_ZN13OriginBackendE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "OriginBackend",
        "connected",
        "",
        "disconnected",
        "statusUpdated",
        "imageReady",
        "cameraModeChanged",
        "isManual",
        "captureParametersChanged",
        "exposure",
        "iso",
        "binning",
        "cameraInfoReceived",
        "cameraID",
        "model",
        "snapshotRequested",
        "tiffImageDownloaded",
        "filePath",
        "imageData",
        "ra",
        "dec",
        "liveImageDownloaded",
        "onWebSocketConnected",
        "onWebSocketDisconnected",
        "onTextMessageReceived",
        "message",
        "updateStatus"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'statusUpdated'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'imageReady'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cameraModeChanged'
        QtMocHelpers::SignalData<void(bool)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 7 },
        }}),
        // Signal 'captureParametersChanged'
        QtMocHelpers::SignalData<void(double, int, int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Double, 9 }, { QMetaType::Int, 10 }, { QMetaType::Int, 11 },
        }}),
        // Signal 'cameraInfoReceived'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 14 },
        }}),
        // Signal 'snapshotRequested'
        QtMocHelpers::SignalData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'tiffImageDownloaded'
        QtMocHelpers::SignalData<void(const QString &, const QByteArray &, double, double, double)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 17 }, { QMetaType::QByteArray, 18 }, { QMetaType::Double, 19 }, { QMetaType::Double, 20 },
            { QMetaType::Double, 9 },
        }}),
        // Signal 'liveImageDownloaded'
        QtMocHelpers::SignalData<void(const QByteArray &, double, double, double)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 18 }, { QMetaType::Double, 19 }, { QMetaType::Double, 20 }, { QMetaType::Double, 9 },
        }}),
        // Slot 'onWebSocketConnected'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onWebSocketDisconnected'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTextMessageReceived'
        QtMocHelpers::SlotData<void(const QString &)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 25 },
        }}),
        // Slot 'updateStatus'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<OriginBackend, qt_meta_tag_ZN13OriginBackendE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject OriginBackend::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13OriginBackendE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13OriginBackendE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13OriginBackendE_t>.metaTypes,
    nullptr
} };

void OriginBackend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<OriginBackend *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->statusUpdated(); break;
        case 3: _t->imageReady(); break;
        case 4: _t->cameraModeChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->captureParametersChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3]))); break;
        case 6: _t->cameraInfoReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->snapshotRequested(); break;
        case 8: _t->tiffImageDownloaded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[5]))); break;
        case 9: _t->liveImageDownloaded((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[4]))); break;
        case 10: _t->onWebSocketConnected(); break;
        case 11: _t->onWebSocketDisconnected(); break;
        case 12: _t->onTextMessageReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->updateStatus(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)()>(_a, &OriginBackend::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)()>(_a, &OriginBackend::disconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)()>(_a, &OriginBackend::statusUpdated, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)()>(_a, &OriginBackend::imageReady, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)(bool )>(_a, &OriginBackend::cameraModeChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)(double , int , int )>(_a, &OriginBackend::captureParametersChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)(const QString & , const QString & )>(_a, &OriginBackend::cameraInfoReceived, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)()>(_a, &OriginBackend::snapshotRequested, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)(const QString & , const QByteArray & , double , double , double )>(_a, &OriginBackend::tiffImageDownloaded, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (OriginBackend::*)(const QByteArray & , double , double , double )>(_a, &OriginBackend::liveImageDownloaded, 9))
            return;
    }
}

const QMetaObject *OriginBackend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OriginBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13OriginBackendE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int OriginBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void OriginBackend::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void OriginBackend::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void OriginBackend::statusUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void OriginBackend::imageReady()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void OriginBackend::cameraModeChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void OriginBackend::captureParametersChanged(double _t1, int _t2, int _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2, _t3);
}

// SIGNAL 6
void OriginBackend::cameraInfoReceived(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void OriginBackend::snapshotRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void OriginBackend::tiffImageDownloaded(const QString & _t1, const QByteArray & _t2, double _t3, double _t4, double _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 9
void OriginBackend::liveImageDownloaded(const QByteArray & _t1, double _t2, double _t3, double _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2, _t3, _t4);
}
QT_WARNING_POP
