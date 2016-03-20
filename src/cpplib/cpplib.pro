QT       += network qml
QT       -= gui

TARGET = iot
TEMPLATE = lib
DEFINES += MAKE_IOTLIB
CONFIG += c++11

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
    endianhelper.h

SOURCES += \
    coap/coap.cpp \
    coap/coapendpoint.cpp \
    coap/coapexchange.cpp \
    coap/coapmessage.cpp \
    coap/timerqueue.cpp
