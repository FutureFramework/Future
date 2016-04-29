QT       += network qml
QT       -= gui

TARGET = iot
TEMPLATE = lib
DEFINES += MAKE_IOTLIB
CONFIG += c++11

include(../../3rdparty/qmsgpack/qmsgpack.pri)
INCLUDEPATH += ../../3rdparty

DESTDIR = $$TOP_SRCDIR/bin

HEADERS += \
    iotlib_global.h \
    coap/coap.hpp \
    coap/coapendpoint.hpp \
    coap/coapendpoint_p.hpp \
    coap/coapexchange.hpp \
    coap/coapexchange_p.hpp \
    coap/coapmessage.hpp \
    coap/timerqueue.hpp \
    endianhelper.h \
    coap/coapcontenthandlers.h

SOURCES += \
    coap/coap.cpp \
    coap/coapendpoint.cpp \
    coap/coapexchange.cpp \
    coap/coapmessage.cpp \
    coap/timerqueue.cpp \
    coap/coapcontenthandlers.cpp
