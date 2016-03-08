QT       += network qml
QT       -= gui

TARGET = iot
TEMPLATE = lib
DEFINES += MAKE_IOTLIB
CONFIG += c++11

DESTDIR = $$TOP_SRCDIR/bin

HEADERS += \
    iotlib_global.h \
    coap/coap.hpp

SOURCES += \
    coap/coap.cpp
