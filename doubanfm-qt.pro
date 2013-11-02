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
    libnotify-qt/OrgFreedesktopNotificationsInterface.cpp \
    albumwidget.cpp \
    mprisplayeradapter.cpp \
    mprisadapter.cpp \
    doubanplayer.cpp \
    doubanmprisplugin.cpp

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
    libnotify-qt/OrgFreedesktopNotificationsInterface.h \
    albumwidget.h \
    mprisplayeradapter.h \
    mprisadapter.h \
    doubanplayer.h \
    doubanmprisplugin.h

FORMS    += mainwidget.ui \
    channelwidget.ui \
    controlpanel.ui \
    loginpanel.ui \
    volumetimepanel.ui \
    pausemask.ui \
    lyricwidget.ui \
    albumwidget.ui

RESOURCES += \
    imgs.qrc

CONFIG += c++11

OTHER_FILES += \
    org.mpris.MediaPlayer2.xml \
    org.mpris.MediaPlayer2.Player.xml \
    org.freedesktop.DBus.Properties.xml
