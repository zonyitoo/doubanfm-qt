#!/bin/sh

SHARE=/usr/share
PIXMAP=/usr/share/pixmaps
APP=/usr/share/applications
BIN=/usr/bin

if [ "$1" = "-u" ]; then
    rm $BIN/doubanfm-qt
    rm $PIXMAP/QDoubanFM.png
    rm $APP/QDoubanFM.desktop
    rm -r $SHARE/QDoubanFM
else
    mkdir -p $SHARE/QDoubanFM
    cp doubanfm-qt $SHARE/QDoubanFM
    #mkdir -p $SHARE/QDoubanFM/lang/
    #cp -r lang/*.qm $SHARE/QDoubanFM/lang/
    rm -f $BIN/doubanfm-qt
    ln -s $SHARE/QDoubanFM/doubanfm-qt $BIN/doubanfm-qt
    cp QDoubanFM.desktop $APP
    cp QDoubanFM.png $PIXMAP
fi
