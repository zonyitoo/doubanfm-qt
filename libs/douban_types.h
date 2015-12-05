#ifndef DOUBAN_TYPES_H
#define DOUBAN_TYPES_H

#include <QtCore>

struct DoubanUser {
    QString user_id;
    QString expire;
    QString token;
    QString user_name;
    QString email;
    QString password;
};

struct DoubanChannel {
    QString name;
    qint32 seq_id;
    QString abbr_en;
    qint32 channel_id;
    QString name_en;
};

struct DoubanFMSong {
    QString album;
    QString picture;
    QString ssid;
    QString artist;
    QString url;
    QString company;
    QString title;
    double rating_avg;
    quint32 length;
    QString subtype;
    QString public_time;
    quint32 sid;
    quint32 aid;
    quint32 kbps;
    QString albumtitle;
    bool like;
};

#endif  // DOUBAN_TYPES_H
