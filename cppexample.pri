QT       += core network
QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle
QMAKE_CXXFLAGS += -std=c++11
DESTDIR = $$TOP_SRCDIR/bin

LIBS += -L"$$TOP_SRCDIR/bin" -liot
INCLUDEPATH += $$TOP_SRCDIR/src/cpplib

TEMPLATE = app