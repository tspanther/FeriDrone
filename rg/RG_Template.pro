QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RG_Template
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    objloader.cpp \
    widgetopengldraw.cpp

HEADERS  += mainwindow.h \
    objloader.hpp \
    widgetopengldraw.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++0x
CONFIG += c++11

# Spremenjeno.
INCLUDEPATH += C:\libs_and_includes\Other\glm

DISTFILES += \
    fsencilnik.frag \
    vsencilnik.vert
