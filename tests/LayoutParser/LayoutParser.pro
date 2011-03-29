#-------------------------------------------------
#
# Project created by QtCreator 2011-03-26T11:44:56
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_layoutparsertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_layoutparsertest.cpp \
    ../../layout-parser/layoutparser.cpp \
    ../../layout-parser/keyboard.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += \
    ../../layout-parser

HEADERS += \
    ../../layout-parser/layoutparser.h \
    ../../layout-parser/keyboard.h
