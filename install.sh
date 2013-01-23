#!/bin/sh

if [ $1 = "-u" ]; then
    rm /usr/bin/doubanfm-qt
    rm /usr/share/pixmaps/QDoubanFM.png
    rm /usr/share/applications/QDoubanFM.desktop
else
    cp doubanfm-qt /usr/bin
    cp QDoubanFM.desktop /usr/share/applications
    cp QDoubanFM.png /usr/share/pixmaps
fi
