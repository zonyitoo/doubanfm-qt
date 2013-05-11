#include "douban.h"
#include <qjson/parser.h>
#include <QtNetwork/QtNetwork>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>
#include <QTime>
#include <QtGui>
#include <QEventLoop>

#include <algorithm>

static const QString DOUBAN_FM_API_ADDR = "http://www.douban.com/j/app/radio/people";
static const QString DOUBAN_FM_API_CHANNEL = "http://www.douban.com/j/app/radio/channels";
static const QString DOUBAN_FM_LOGIN = "http://www.douban.com/j/app/login";

Douban::Douban(QObject *parent) : QObject(parent) {
    for (size_t i = 0; i < DOUBAN_MANAGER_ARRAY_SIZE; ++ i)
        _managers[i] = NULL;
}

Douban::~Douban() {
    for (size_t i = 0; i < DOUBAN_MANAGER_ARRAY_SIZE; ++ i)
        delete _managers[i];
}

Douban* Douban::getInstance() {
    static Douban *_INSTANCE = new Douban();
    return _INSTANCE;
}

void Douban::onLoginSucceed(DoubanUser user) {
    _user = user;
    emit this->loginSucceed(_user);
}

void Douban::doLogin(const QString &email, const QString &password) {
    QString args = QString("app_name=radio_desktop_win&version=100")
            + QString("&email=") + email
            + QString("&password=") + password;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QVariant("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(args.length()));
    request.setUrl(QUrl(DOUBAN_FM_LOGIN));
    if (_managers[8] == NULL) {
        _managers[8] = new QNetworkAccessManager(this);
        connect(_managers[8], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedAuth(QNetworkReply*)));
    }
    _managers[8]->post(request, args.toAscii());
}

void Douban::userReLogin() {
    QString args = QString("app_name=radio_desktop_win&version=100")
            + QString("&email=") + _user.email
            + QString("&password=") + _user.password;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QVariant("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(args.length()));
    request.setUrl(QUrl(DOUBAN_FM_LOGIN));
    if (_managers[0] == NULL) {
        _managers[0] = new QNetworkAccessManager(this);
        connect(_managers[0], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedRelogin(QNetworkReply*)));
    }
    _managers[0]->post(request, args.toAscii());

    QEventLoop eventloop;
    connect(_managers[0], SIGNAL(finished(QNetworkReply*)), &eventloop, SLOT(quit()));
    eventloop.exec();
}

void Douban::onReceivedRelogin(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    if (ok) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            qDebug() << Q_FUNC_INFO << "Err" << obj["err"].toString();
            return;
        }
        DoubanUser nuser;
        nuser.user_id = obj["user_id"].toString();
        nuser.expire = obj["expire"].toString();
        nuser.token = obj["token"].toString();
        nuser.user_name = obj["user_name"].toString();
        nuser.email = obj["email"].toString();
        nuser.password = _user.password;

        _user = nuser;
    }

    reply->deleteLater();
}

void Douban::onReceivedAuth(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    if (ok) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            qDebug() << Q_FUNC_INFO << "Err" << obj["err"].toString();
            emit loginFailed(obj["err"].toString());
            return;
        }
        DoubanUser nuser;
        nuser.user_id = obj["user_id"].toString();
        nuser.expire = obj["expire"].toString();
        nuser.token = obj["token"].toString();
        nuser.user_name = obj["user_name"].toString();
        nuser.email = obj["email"].toString();

        onLoginSucceed(nuser);
    }

    reply->deleteLater();
}

void Douban::userLogout() {
    _user = DoubanUser();
    emit logoffSucceed();
}

void Douban::setUser(const DoubanUser& user) {
    _user = user;

    if (_user.user_id.isEmpty()
            || _user.token.isEmpty()
            || _user.user_id.isEmpty()
            || _user.expire.isEmpty()) {
        //_user = DoubanUser();
        return;
    }

    QTime time;
    if (_user.expire.toInt() <= time.msec()) {
        this->userReLogin();
    }
}

DoubanUser Douban::getUser() {
    return _user;
}

void Douban::getNewPlayList(const quint32& channel) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + _user.user_id
            + QString("&expire=") + _user.expire
            + QString("&token=") + _user.token
            + QString("&sid=&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=n");

    if (_managers[1] == NULL) {
        _managers[1] = new QNetworkAccessManager(this);
        connect(_managers[1], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedNewList(QNetworkReply*)));
    }
    _managers[1]->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void Douban::onReceivedNewList(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    QList<DoubanFMSong> songs;

    if (ok) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            if (obj["err"].toString() == "expired") {
                qDebug() << Q_FUNC_INFO << "User expired. Relogin";
                userReLogin();
            }
            else
                qDebug() << Q_FUNC_INFO << "Err" << obj["err"].toString();
            return;
        }
        QVariantList songList = obj["song"].toList();
        foreach(const QVariant& item, songList) {
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
            s.sid = song["sid"].toUInt();
            s.aid = song["aid"].toUInt();
            s.albumtitle = song["albumtitle"].toString();
            s.like = song["like"].toBool();
            songs.push_back(s);
        }
        qDebug() << Q_FUNC_INFO << "songs.size() =" << songs.size();
    }
    emit receivedNewList(songs);
    reply->deleteLater();
}

void Douban::getPlayingList(const quint32& channel, const quint32 &sid) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + _user.user_id
            + QString("&expire=") + _user.expire
            + QString("&token=") + _user.token
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=p");

    if (_managers[7] == NULL) {
        _managers[7] = new QNetworkAccessManager(this);
        connect(_managers[7], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedPlayingList(QNetworkReply*)));
    }
    _managers[7]->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void Douban::onReceivedPlayingList(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    QList<DoubanFMSong> songs;

    if (ok) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            if (obj["err"].toString() == "expired") {
                qDebug() << Q_FUNC_INFO << "User expired. Relogin";
                userReLogin();
            }
            else
                qDebug() << Q_FUNC_INFO << "Err" << obj["err"].toString();
            return;
        }
        QVariantList songList = obj["song"].toList();
        foreach(const QVariant& item, songList) {
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
            s.sid = song["sid"].toUInt();
            s.aid = song["aid"].toUInt();
            s.albumtitle = song["albumtitle"].toString();
            s.like = song["like"].toBool();
            songs.push_back(s);
        }
        qDebug() << Q_FUNC_INFO << "songs.size() =" << songs.size();
    }
    emit receivedPlayingList(songs);
    reply->deleteLater();
}

void Douban::rateSong(const quint32& sid, const quint32 &channel, const bool toRate) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + _user.user_id
            + QString("&expire=") + _user.expire
            + QString("&token=") + _user.token
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + (toRate? QString("&type=r"): QString("&type=u"));
    if (_managers[2] == NULL) {
        _managers[2] = new QNetworkAccessManager(this);
        connect(_managers[2], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedRateSong(QNetworkReply*)));
    }
    _managers[2]->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void Douban::onReceivedRateSong(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    if (ok) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() == 1) {
            if (obj["err"].toString() == "expired") {
                qDebug() << Q_FUNC_INFO << "User expired. Relogin";
                userReLogin();
            }
            emit receivedRateSong(false);
        }
        else {
            emit receivedRateSong(true);
        }
    }
    reply->deleteLater();
}

void Douban::skipSong(const quint32 &sid, const quint32& channel) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + _user.user_id
            + QString("&expire=") + _user.expire
            + QString("&token=") + _user.token
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=s");
    if (_managers[3] == NULL) {
        _managers[3] = new QNetworkAccessManager(this);
        connect(_managers[3], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedSkipSong(QNetworkReply*)));
    }
    _managers[3]->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void Douban::onReceivedSkipSong(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    if (ok) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() == 1) {
            if (obj["err"].toString() == "expired") {
                qDebug() << Q_FUNC_INFO << "User expired. Relogin";
                userReLogin();
            }
            emit receivedSkipSong(false);
        }
        else {
            emit receivedSkipSong(true);
        }
    }
    reply->deleteLater();
}

void Douban::songEnd(const quint32& sid, const quint32& channel) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + _user.user_id
            + QString("&expire=") + _user.expire
            + QString("&token=") + _user.token
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=e");
    if (_managers[4] == NULL) {
        _managers[4] = new QNetworkAccessManager(this);
        connect(_managers[4], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedCurrentEnd(QNetworkReply*)));
    }
    _managers[4]->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void Douban::onReceivedCurrentEnd(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    if (all == "ok") emit receivedCurrentEnd(true);
    else emit receivedCurrentEnd(false);
}

void Douban::byeSong(const quint32 &sid, const quint32 &channel) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + _user.user_id
            + QString("&expire=") + _user.expire
            + QString("&token=") + _user.token
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=b");
    if (_managers[5] == NULL) {
        _managers[5] = new QNetworkAccessManager(this);
        connect(_managers[5], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedByeSong(QNetworkReply*)));
    }
    _managers[5]->get(QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args)));
}

void Douban::onReceivedByeSong(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    if (ok) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() == 1) {
            if (obj["err"].toString() == "expired") {
                qDebug() << Q_FUNC_INFO << "User expired. Relogin";
                userReLogin();
            }
            emit receivedByeSong(false);
        }
        else {
            emit receivedByeSong(true);
        }
    }
    reply->deleteLater();
}

void Douban::getChannels() {
    if (_managers[6] == NULL) {
        _managers[6] = new QNetworkAccessManager(this);
        connect(_managers[6], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedChannels(QNetworkReply*)));
    }
    _managers[6]->get(QNetworkRequest(QUrl(DOUBAN_FM_API_CHANNEL)));
}

bool cmp_channels(const DoubanChannel& a, const DoubanChannel& b) {
    return a.channel_id < b.channel_id;
}

void Douban::onReceivedChannels(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    QList<DoubanChannel> channels;

    if (ok) {
        QVariantMap obj = result.toMap();
        QVariantList chList = obj["channels"].toList();
        foreach(const QVariant& item, chList) {
            QVariantMap ch = item.toMap();
            DoubanChannel dc;
            dc.name = ch["name"].toString();
            dc.name_en = ch["name_en"].toString();
            dc.seq_id = ch["seq_id"].toUInt();
            dc.channel_id = ch["channel_id"].toUInt();
            dc.abbr_en = ch["abbr_en"].toString();
            channels.append(dc);
        }
        qDebug() << Q_FUNC_INFO << "channels.size() =" << channels.size();
    }

    std::sort(channels.begin(), channels.end(), cmp_channels);
    emit receivedChannels(channels);
    reply->deleteLater();
}

bool Douban::hasLogin() {
    return (!_user.expire.isEmpty()
            && !_user.token.isEmpty()
            && !_user.user_id.isEmpty()
            && !_user.email.isEmpty()
            && !_user.user_name.isEmpty());
}
