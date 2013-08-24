#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T21:55:51
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = doubanfm-qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    channelwidget.cpp \
    controlpanel.cpp \
    doubanfm.cpp \
    horizontalslider.cpp \
    loginpanel.cpp \
    volumetimepanel.cpp \
    pausemask.cpp

HEADERS  += mainwidget.h \
    channelwidget.h \
    controlpanel.h \
    douban_types.h \
    doubanfm.h \
    horizontalslider.h \
    loginpanel.h \
    volumetimepanel.h \
    pausemask.h

FORMS    += mainwidget.ui \
    channelwidget.ui \
    controlpanel.ui \
    loginpanel.ui \
    volumetimepanel.ui \
    pausemask.ui

RESOURCES += \
    imgs.qrc

CONFIG += c++11
