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
        src/mainwindow.cpp \
    src/douban.cpp \
    src/doubanlogindialog.cpp

HEADERS  += include/mainwindow.h \
    include/douban.h \
    include/doubanlogindialog.h \
    include/douban_types.h

FORMS    += src/mainwindow.ui \
    src/doubanlogindialog.ui

LIBS     += -lqjson -lphonon

INCLUDEPATH += include/
