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
    src/channelwidget.cpp \
    src/controlpanel.cpp \
    src/mainwidget.cpp \
    src/maskpausewidget.cpp \
    src/volumewidget.cpp \
    src/userloginwidget.cpp

HEADERS  += \
    include/douban.h \
    include/douban_types.h \
    include/channelwidget.h \
    include/controlpanel.h \
    include/mainwidget.h \
    include/maskpausewidget.h \
    include/volumewidget.h \
    include/userloginwidget.h

FORMS    += \
    ui/channelwidget.ui \
    ui/controlpanel.ui \
    ui/mainwidget.ui \
    ui/volumewidget.ui \
    ui/userloginwidget.ui

LIBS     += -lqjson -lphonon

INCLUDEPATH += include/

RESOURCES += \
    icons.qrc

TRANSLATIONS += lang/China_Chinese.ts \
                lang/Taiwan_Chinese.ts
