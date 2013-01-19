#include "networkmanager.h"
#include <qjson/parser.h>

static const QString DOUBAN_FM_API_ADDR = "http://www.douban.com/j/app/radio/people";

NetworkManager::NetworkManager(QWidget *widget) {
    _manager = new QNetworkAccessManager(widget);
}

NetworkManager::~NetworkManager() {
    delete _manager;
}

void NetworkManager::getNewList(const int channel, const DoubanUser& user) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + user.getUserId()
            + QString("&expire=") + user.getExpire()
            + QString("&token=") + user.getExpire()
            + QString("&sid=&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=n");
    disconnect(_manager, 0, 0, 0);
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReceivedNewList(QNetworkReply*)));
    _manager->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void NetworkManager::onReceivedNewList(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    QList<DoubanFMSong> songs;

    if (ok) {
        QVariantMap obj = result.toMap();
        QVariantList songList = obj["song"].toList();
        songs.clear();
        foreach(QVariant item, songList) {
            QVariantMap song = item.toMap();
            DoubanFMSong s;
            s.album = song["album"].toString();
            s.picture = song["picture"].toString();
            s.ssid = song["ssid"].toString();
            s.artist = song["artist"].toString();
            s.url = song["url"].toString();
            s.company = song["company"].toString();
            s.title = song["title"].toString();
            s.public_time = song["public_time"].toString();
            s.sid = song["sid"].toString();
            s.aid = song["aid"].toString();
            s.albumtitle = song["albumtitle"].toString();
            s.like = song["like"].toBool();
            songs.push_back(s);
        }
        qDebug() << Q_FUNC_INFO << "songs.size()=" << songs.size();
    }
    emit receivedNewList(songs);
    reply->deleteLater();
}

void NetworkManager::getPlayingList(const int channel,
                                    const QList<DoubanFMSong> &historyList,
                                    const DoubanUser &user) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + user.getUserId()
            + QString("&expire=") + user.getExpire()
            + QString("&token=") + user.getExpire()
            + QString("&sid=")
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=p");
    disconnect(_manager, 0, 0, 0);
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReceivedPlayingList(QNetworkReply*)));
    _manager->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void NetworkManager::onReceivedPlayingList(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    QList<DoubanFMSong> songs;

    if (ok) {
        QVariantMap obj = result.toMap();
        QVariantList songList = obj["song"].toList();
        songs.clear();
        foreach(QVariant item, songList) {
            QVariantMap song = item.toMap();
            DoubanFMSong s;
            s.album = song["album"].toString();
            s.picture = song["picture"].toString();
            s.ssid = song["ssid"].toString();
            s.artist = song["artist"].toString();
            s.url = song["url"].toString();
            s.company = song["company"].toString();
            s.title = song["title"].toString();
            s.public_time = song["public_time"].toString();
            s.sid = song["sid"].toString();
            s.aid = song["aid"].toString();
            s.albumtitle = song["albumtitle"].toString();
            s.like = song["like"].toBool();
            songs.push_back(s);
        }
        qDebug() << Q_FUNC_INFO << "songs.size()=" << songs.size();
    }
    emit receivedPlayingList(songs);
    reply->deleteLater();
}

void NetworkManager::rateSong(const int channel,
              const QString& sid,
              const DoubanUser& user) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + user.getUserId()
            + QString("&expire=") + user.getExpire()
            + QString("&token=") + user.getExpire()
            + QString("&sid=") + sid
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=p");
    disconnect(_manager, 0, 0, 0);
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReceivedRateSong(QNetworkReply*)));
    _manager->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void NetworkManager::onReceivedRateSong(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    QList<DoubanFMSong> songs;

    if (ok) {
        QVariantMap obj = result.toMap();
        QVariantList songList = obj["song"].toList();
        songs.clear();
        foreach(QVariant item, songList) {
            QVariantMap song = item.toMap();
            DoubanFMSong s;
            s.album = song["album"].toString();
            s.picture = song["picture"].toString();
            s.ssid = song["ssid"].toString();
            s.artist = song["artist"].toString();
            s.url = song["url"].toString();
            s.company = song["company"].toString();
            s.title = song["title"].toString();
            s.public_time = song["public_time"].toString();
            s.sid = song["sid"].toString();
            s.aid = song["aid"].toString();
            s.albumtitle = song["albumtitle"].toString();
            s.like = song["like"].toBool();
            songs.push_back(s);
        }
        qDebug() << Q_FUNC_INFO << "songs.size()=" << songs.size();
    }
    emit receivedRateSong(songs);
    reply->deleteLater();
}

void NetworkManager::unrateSong(const int channel,
                const QString& sid,
                const DoubanUser& user) {

}

void NetworkManager::onReceivedUnrateSong(QNetworkReply *reply) {

}

void NetworkManager::skipSong(const int channel,
              const QString& sid,
              const DoubanUser& user) {

}

void NetworkManager::onReceivedSkipSong(QNetworkReply *reply) {

}

void NetworkManager::currentEnd(const int channel,
                const QString& sid,
                const DoubanUser& user) {

}

void NetworkManager::onReceivedCurrentEnd(QNetworkReply *reply) {

}

void NetworkManager::byeSong(const int channel,
             const QString& sid,
             const DoubanUser& user) {

}

void NetworkManager::onReceivedByeSong(QNetworkReply *reply) {

}

void NetworkManager::getImage(const QString &url) {
    qDebug() << Q_FUNC_INFO << url;
    disconnect(_manager, 0, 0, 0);
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReceivedImage(QNetworkReply*)));
    _manager->get(QNetworkRequest(QUrl(url)));
}

void NetworkManager::onReceivedImage(QNetworkReply *reply) {
    if (QNetworkReply::NoError != reply->error()) {
        qDebug() << Q_FUNC_INFO << "pixmap receiving error" << reply->error();
        reply->deleteLater();
        return;
    }
    const QByteArray data(reply->readAll());
    if (!data.size())
        qDebug() << Q_FUNC_INFO << "received pixmap looks like nothing";
    emit receivedImage(data);
    reply->deleteLater();
}
