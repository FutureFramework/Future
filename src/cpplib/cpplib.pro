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
    coap/timerqueue.hpp \
    endianhelper.h \
    coap/message.hpp \
    coap/exchange_p.hpp \
    coap/exchange.hpp \
    coap/stack_p.hpp \
    coap/stack.hpp \
    coap/contenthandlers.h \
    coap/endpointbase.hpp \
    settings.h \
    coap/udpendpoint.h

SOURCES += \
    coap/coap.cpp \
    coap/timerqueue.cpp \
    coap/stack.cpp \
    coap/message.cpp \
    coap/exchange.cpp \
    coap/contenthandlers.cpp \
    settings.cpp \
    coap/udpendpoint.cpp
