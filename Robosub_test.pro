#-------------------------------------------------
#
# Project created by QtCreator 2017-05-02T17:15:51
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Robosub_test
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        dialog.cpp \
    remotedialog.cpp \
    piddialog.cpp \
    configmanager.cpp \
    controller.cpp \
    transceiver.cpp \
    visionclass.cpp \
    camclass.cpp \
    missionflare.cpp \
    missionacquire.cpp \
    missiondrop.cpp \
    recordmanager.cpp \
    templatematch_vector.cpp \
    colorrecognize.cpp \
    rectboundary.cpp \
    visiondialog.cpp \
    missiongate.cpp \
    capturethread.cpp

HEADERS  += dialog.h \
    include/ImageLib.h \
    include/PvApi.h \
    include/PvRegIo.h \
    include/JHCap.h \
    include/imageutil.h \
    include/capturethread.h \
    global.h \
    remotedialog.h \
    piddialog.h \
    configmanager.h \
    controller.h \
    transceiver.h \
    visionclass.h \
    camclass.h \
    missionacquire.h \
    missiondrop.h \
    missionflare.h \
    recordmanager.h \
    missiongate.h \
    visiondialog.h \
    ui_dialog.h

CONFIG += link_pkgconfig

CONFIG += c++14

QMAKE_CXXFLAGS += -funsigned-char

unix:!macx: LIBS += -L$$PWD/lib -lImagelib

INCLUDEPATH += $$PWD/include \
    /usr/
DEPENDPATH += $$PWD/include

unix:!macx: PRE_TARGETDEPS += $$PWD/lib/libImagelib.a

unix:!macx: LIBS += -L$$PWD/lib -lPvAPI

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

unix:!macx: PRE_TARGETDEPS += $$PWD/lib/libPvAPI.a

LIBS += -lpthread \
    -lz \
    -lrt \
    -lm \
    -lJHCap \
    `pkg-config --libs opencv`

FORMS +=

