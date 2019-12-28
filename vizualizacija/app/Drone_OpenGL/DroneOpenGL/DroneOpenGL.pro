QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RG_Template
TEMPLATE = app

SOURCES += main.cpp\
        camera.cpp \
        mainwindow.cpp \
        objekt.cpp \
        widgetopengldraw.cpp

HEADERS  += mainwindow.h \
        camera.h \
        objekt.h \
        widgetopengldraw.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++0x
CONFIG += c++11

INCLUDEPATH += ../../lib/glm/
INCLUDEPATH += ../../lib/objectLoader/

DISTFILES += \
    fshader.frag \
    vshader.vert
