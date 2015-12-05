#include "lyricgetter.h"
#include <QUrl>
#include <QNetworkReply>
#include <QJsonDocument>

static const QString LYRIC_API = "http://api.douban.com/v2/fm/lyric";

LyricGetter::LyricGetter(QObject *parent) : QObject(parent) {
    querymgr = new QNetworkAccessManager(this);
    connect(querymgr,
            &QNetworkAccessManager::finished,
            [this](QNetworkReply *reply) {
                if (QNetworkReply::NoError != reply->error()) {
                    qDebug() << "Lyric not found. Err " << qPrintable(reply->errorString());
                    reply->deleteLater();
                    emit gotLyricError("Lyric doesn't exist");
                    return;
                }
                const QTextCodec *codec = QTextCodec::codecForName("utf-8");
                QString all             = codec->toUnicode(reply->readAll());
                QJsonParseError parseerr;
                QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();

                if (parseerr.error != QJsonParseError::ParseError::NoError) {
                    qDebug() << "Lyric response parse error: " << qPrintable(parseerr.errorString());
                    emit gotLyricError("Lyric doesn't exist");
                    return;
                }

                QVariantMap obj = result.toMap();

                if (obj.find("lyric") == obj.end()) {
                    qDebug() << "Could not find lyric";
                    emit gotLyricError("Lyric doesn't exist");
                } else {
                    qDebug() << "Got lyric for " << qPrintable(obj["name"].toString());
                    QString lyric_str = obj["lyric"].toString();
                    QTextStream lyric(&lyric_str);
                    emit gotLyric(QLyricParser::parse(lyric));
                }

                reply->deleteLater();
            });
}

LyricGetter::~LyricGetter() { delete querymgr; }

void LyricGetter::getLyric(const DoubanFMSong &song) {
    qDebug() << "Going to get lyric for " << qPrintable(song.artist) << " " << qPrintable(song.title);

    auto request = QNetworkRequest(QUrl(LYRIC_API));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery postData;
    postData.addQueryItem("sid", QString::number(song.sid));
    postData.addQueryItem("ssid", song.ssid);

    querymgr->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
}
