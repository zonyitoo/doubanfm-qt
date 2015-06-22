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
            qDebug() << "Lyric not found.";
            reply->deleteLater();
            emit gotLyricError("Lyric doesn't exist.");
            return;
        }
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QString all = codec->toUnicode(reply->readAll());
        qDebug() << "Lyric query: " << all;
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
            qDebug() << "Lyric not found.";
            reply->deleteLater();
            return;
        }

        QTextStream stream(reply->readAll());
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        stream.setCodec(codec);
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

    qDebug() << "Going to get lyric for " << artist << " " << song;

    auto request = QNetworkRequest(QUrl(fullurl));
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    querymgr->get(request);
}
