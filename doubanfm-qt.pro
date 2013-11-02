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
    libs/doubanfm.cpp \
    horizontalslider.cpp \
    loginpanel.cpp \
    volumetimepanel.cpp \
    pausemask.cpp \
    libs/qlyricparser.cpp \
    libs/lyricgetter.cpp \
    lyricwidget.cpp \
    albumimage.cpp \
    triggerarea.cpp \
    libnotify-qt/Notification.cpp \
    libnotify-qt/OrgFreedesktopNotificationsInterface.cpp \
    albumwidget.cpp \
    plugins/mpris/mprisplayeradapter.cpp \
    plugins/mpris/mprisadapter.cpp \
    libs/doubanplayer.cpp \
    plugins/mpris/doubanmprisplugin.cpp \
    settingdialog.cpp

HEADERS  += mainwidget.h \
    channelwidget.h \
    controlpanel.h \
    libs/douban_types.h \
    libs/doubanfm.h \
    horizontalslider.h \
    loginpanel.h \
    volumetimepanel.h \
    pausemask.h \
    libs/qlyricparser.h \
    libs/lyricgetter.h \
    lyricwidget.h \
    albumimage.h \
    triggerarea.h \
    channelwidgettrigger.h \
    lyricwidgettrigger.h \
    libnotify-qt/Notification.h \
    libnotify-qt/OrgFreedesktopNotificationsInterface.h \
    albumwidget.h \
    plugins/mpris/mprisplayeradapter.h \
    plugins/mpris/mprisadapter.h \
    libs/doubanplayer.h \
    plugins/mpris/doubanmprisplugin.h \
    settingdialog.h

FORMS    += mainwidget.ui \
    channelwidget.ui \
    controlpanel.ui \
    loginpanel.ui \
    volumetimepanel.ui \
    pausemask.ui \
    lyricwidget.ui \
    albumwidget.ui \
    settingdialog.ui

RESOURCES += \
    imgs.qrc

CONFIG += c++11

OTHER_FILES += \
    plugins/org.mpris.MediaPlayer2.xml \
    plugins/org.mpris.MediaPlayer2.Player.xml \
    plugins/org.freedesktop.DBus.Properties.xml
