#-------------------------------------------------
#
# Project created by QtCreator 2013-01-19T17:24:30
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = doubanfm-ubuntu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    douban.cpp

HEADERS  += mainwindow.h \
    douban.h

FORMS    += mainwindow.ui

LIBS     += -lqjson -lphonon
