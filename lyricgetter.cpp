#include "lyricgetter.h"
#include <QUrl>
#include <QNetworkReply>
#include <QJsonDocument>

static const QString LYRIC_API = "http://geci.me/api/lyric";

LyricGetter::LyricGetter(QObject *parent) :
    QObject(parent)
{
    querymgr = new QNetworkAccessManager(this);
    getmgr = new QNetworkAccessManager(this);
    connect(querymgr, &QNetworkAccessManager::finished, [this] (QNetworkReply *reply) {
        if (QNetworkReply::NoError != reply->error()) {
            qDebug() << "Err: Get lyric error";
            reply->deleteLater();
            return;
        }
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QString all = codec->toUnicode(reply->readAll());
        QJsonParseError parseerr;
        QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();
        QVariantMap obj = result.toMap();
        if (obj["count"].toInt() != 0) {
            QVariantList resultlist = obj["result"].toList();
            QVariantMap first = resultlist[0].toMap();
            getmgr->get(QNetworkRequest(QUrl(first["lrc"].toString())));
        }

        reply->deleteLater();
    });
    connect(getmgr, &QNetworkAccessManager::finished, [this] (QNetworkReply *reply) {
        if (QNetworkReply::NoError != reply->error()) {
            qDebug() << "Err: Get lyric error";
            reply->deleteLater();
            return;
        }
        QTextStream stream(reply->readAll());
        emit gotLyric(QLyricParser::parse(stream));

        reply->deleteLater();
    });
}

LyricGetter::~LyricGetter() {
    delete querymgr;
    delete getmgr;
}

void LyricGetter::getLyric(const QString &song, const QString &artist) {
    QString fullurl = LYRIC_API + "/" + QUrl::toPercentEncoding(song);
    if (artist.size())
        fullurl += "/" + QUrl::toPercentEncoding(artist);
    querymgr->get(QNetworkRequest(QUrl(fullurl)));
}
