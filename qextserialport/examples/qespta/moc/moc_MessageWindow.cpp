/****************************************************************************
** Meta object code from reading C++ file 'MessageWindow.h'
**
** Created: Fri Dec 7 22:59:46 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MessageWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MessageWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MessageWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_MessageWindow[] = {
    "MessageWindow\0"
};

const QMetaObject MessageWindow::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_MessageWindow,
      qt_meta_data_MessageWindow, 0 }
};

const QMetaObject *MessageWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MessageWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MessageWindow))
	return static_cast<void*>(const_cast< MessageWindow*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int MessageWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
