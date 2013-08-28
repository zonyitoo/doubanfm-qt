#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T21:55:51
#
#-------------------------------------------------

QT       += core gui network multimedia dbus

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
    pausemask.cpp \
    qlyricparser.cpp \
    lyricgetter.cpp \
    lyricwidget.cpp \
    albumimage.cpp \
    triggerarea.cpp \
    libnotify-qt/Notification.cpp \
    libnotify-qt/OrgFreedesktopNotificationsInterface.cpp

HEADERS  += mainwidget.h \
    channelwidget.h \
    controlpanel.h \
    douban_types.h \
    doubanfm.h \
    horizontalslider.h \
    loginpanel.h \
    volumetimepanel.h \
    pausemask.h \
    qlyricparser.h \
    lyricgetter.h \
    lyricwidget.h \
    albumimage.h \
    triggerarea.h \
    channelwidgettrigger.h \
    lyricwidgettrigger.h \
    libnotify-qt/Notification.h \
    libnotify-qt/OrgFreedesktopNotificationsInterface.h

FORMS    += mainwidget.ui \
    channelwidget.ui \
    controlpanel.ui \
    loginpanel.ui \
    volumetimepanel.ui \
    pausemask.ui \
    lyricwidget.ui

RESOURCES += \
    imgs.qrc

CONFIG += c++11
