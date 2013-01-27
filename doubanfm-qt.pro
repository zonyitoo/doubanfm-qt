#-------------------------------------------------
#
# Project created by QtCreator 2013-01-19T17:24:30
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = doubanfm-qt
TEMPLATE = app


SOURCES += src/main.cpp\
    src/douban.cpp \
    src/mainui.cpp \
    src/channelbutton.cpp

HEADERS  += \
    include/douban.h \
    include/douban_types.h \
    include/mainui.h \
    include/channelbutton.h

FORMS    += \
    src/mainui.ui

LIBS     += -lqjson -lphonon

INCLUDEPATH += include/

RESOURCES +=
