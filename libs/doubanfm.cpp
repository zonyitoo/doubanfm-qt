#include "doubanfm.h"
#include <QJsonDocument>
#include <QtNetwork/QtNetwork>
#include <QTime>
#include <QtGui>
#include <QEventLoop>
#include <memory>

#include <algorithm>

static const QString DOUBAN_FM_API_ADDR = "https://www.douban.com/j/app/radio/people";
static const QString DOUBAN_FM_API_CHANNEL = "https://www.douban.com/j/app/radio/channels";
static const QString DOUBAN_FM_LOGIN = "https://www.douban.com/j/app/login";

DoubanFM::DoubanFM(QObject *parent) : QObject(parent) {
    for (size_t i = 0; i < DOUBAN_MANAGER_ARRAY_SIZE; ++ i)
        _managers[i] = nullptr;

    QSettings settings("QDoubanFM", "QDoubanFM");
    settings.beginGroup("User");
    DoubanUser user;
    user.email = settings.value("email", "").toString();
    user.expire = settings.value("expire", "").toString();
    user.password = settings.value("password", "").toString();
    user.token = settings.value("token", "").toString();
    user.user_id = settings.value("user_id", "").toString();
    user.user_name = settings.value("user_name", "").toString();
    if (user.email.size() && user.expire.size()
            && user.password.size() && user.token.size()
            && user.user_id.size() && user.user_name.size())
        this->setUser(user);
    settings.endGroup();
}

DoubanFM::~DoubanFM() {
    this->disconnect();

    for (size_t i = 0; i < DOUBAN_MANAGER_ARRAY_SIZE; ++ i)
        if (_managers[i]) delete _managers[i];

    QSettings settings("QDoubanFM", "QDoubanFM");
    auto user = this->getUser();
    if (!user) return;
    settings.beginGroup("User");
    settings.setValue("email", user->email);
    settings.setValue("expire", user->expire);
    settings.setValue("password", user->password);
    settings.setValue("token", user->token);
    settings.setValue("user_id", user->user_id);
    settings.setValue("user_name", user->user_name);
    settings.endGroup();
    settings.sync();
}

DoubanFM& DoubanFM::getInstance() {
    static DoubanFM _INSTANCE(nullptr);
    return _INSTANCE;
}

void DoubanFM::userLogin(const QString &email, const QString &password) {
    QString args = QString("app_name=radio_desktop_win&version=100")
            + QString("&email=") + email
            + QString("&password=") + QUrl::toPercentEncoding(password);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QVariant("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(args.length()));
    request.setUrl(QUrl(DOUBAN_FM_LOGIN));

    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    if (_managers[8] == nullptr) {
        _managers[8] = new QNetworkAccessManager(this);
        connect(_managers[8], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedAuth(QNetworkReply*)));
    }
    qDebug() << "Login with " << email;
    _managers[8]->post(request, args.toLatin1());
    _user.reset(new DoubanUser);
    _user->password = password;
}

void DoubanFM::userReLogin() {
    if (!_user) return;
    QString args = QString("app_name=radio_desktop_win&version=100")
            + QString("&email=") + _user->email
            + QString("&password=") + QUrl::toPercentEncoding(_user->password);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QVariant("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(args.length()));
    request.setUrl(QUrl(DOUBAN_FM_LOGIN));

    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    if (_managers[0] == nullptr) {
        _managers[0] = new QNetworkAccessManager(this);
        connect(_managers[0], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedRelogin(QNetworkReply*)));
    }
    qDebug() << "Relogin with " << _user->email;
    _managers[0]->post(request, args.toLatin1());

    QEventLoop eventloop;
    connect(_managers[0], SIGNAL(finished(QNetworkReply*)), &eventloop, SLOT(quit()));
    eventloop.exec();
}

void DoubanFM::onReceivedRelogin(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occurred while re-loging in: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJsonParseError parseerr;
    QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();

    if (parseerr.error == QJsonParseError::NoError) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            qDebug() << Q_FUNC_INFO << "ReloginErr: " << obj["err"].toString();
            return;
        }

        _user->user_id = obj["user_id"].toString();
        _user->expire = obj["expire"].toString();
        _user->token = obj["token"].toString();
        _user->user_name = obj["user_name"].toString();
        _user->email = obj["email"].toString();
    }

    reply->deleteLater();
}

void DoubanFM::onReceivedAuth(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occurred while authenticating: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJsonParseError parseerr;
    QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();
    if (parseerr.error == QJsonParseError::NoError) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            qDebug() << Q_FUNC_INFO << "Err" << obj["err"].toString();
            emit loginFailed(obj["err"].toString());
            this->_user.reset();
            reply->deleteLater();
            return;
        }

        _user->user_id = obj["user_id"].toString();
        _user->expire = obj["expire"].toString();
        _user->token = obj["token"].toString();
        _user->user_name = obj["user_name"].toString();
        _user->email = obj["email"].toString();

        emit this->loginSucceed(*_user);
    }

    reply->deleteLater();
}

void DoubanFM::userLogout() {
    _user.reset();
    emit logoffSucceed();
}

void DoubanFM::setUser(const DoubanUser &user) {
    _user.reset(new DoubanUser(user));

    QTime time;
    if (_user->expire.toInt() <= time.msec()) {
        this->userReLogin();
    }
}

const DoubanUser *DoubanFM::getUser() const {
    return _user.get();
}

void DoubanFM::getNewPlayList(const qint32& channel, qint32 kbps) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + ((_user) ? _user->user_id : QString())
            + QString("&expire=") + ((_user) ? _user->expire : QString())
            + QString("&token=") + ((_user) ? _user->token : QString())
            + QString("&sid=&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&kbps=") + QString::number(kbps, 10)
            + QString("&type=n");
    if (_managers[1] == nullptr) {
        _managers[1] = new QNetworkAccessManager(this);
        connect(_managers[1], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedNewList(QNetworkReply*)));
    }

    auto request = QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args));
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    qDebug() << "Getting playlist (new) for channel: " << channel << ", kbps: " << kbps;
    _managers[1]->get(request);
}

void DoubanFM::onReceivedNewList(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occurred while receiving new list: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QList<DoubanFMSong> songs;
    QJsonParseError parseerr;
    QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();

    if (parseerr.error == QJsonParseError::NoError) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            if (obj["err"].toString() == "expired") {
                qDebug() << Q_FUNC_INFO << "User expired. Relogin";
                userReLogin();
            }
            else
                qDebug() << Q_FUNC_INFO << "Err" << obj["err"].toString();
            reply->deleteLater();
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
    }
    emit receivedNewList(songs);
    reply->deleteLater();
}

void DoubanFM::getPlayingList(const qint32 &channel, const quint32 &sid, qint32 kbps) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + ((_user) ? _user->user_id : QString())
            + QString("&expire=") + ((_user) ? _user->expire : QString())
            + QString("&token=") + ((_user) ? _user->token : QString())
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&kbps=") + QString::number(kbps, 10)
            + QString("&type=p");

    if (_managers[7] == nullptr) {
        _managers[7] = new QNetworkAccessManager(this);
        connect(_managers[7], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedPlayingList(QNetworkReply*)));
    }

    auto request = QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args));
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    qDebug() << "Getting playlist (playing) for channel: " << channel << ", sid: " << sid << ", kbps: " << kbps;
    _managers[7]->get(request);
}

void DoubanFM::onReceivedPlayingList(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occurred while receiving playing list: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QList<DoubanFMSong> songs;
    QJsonParseError parseerr;
    QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();

    if (parseerr.error == QJsonParseError::NoError) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            if (obj["err"].toString() == "expired") {
                qDebug() << Q_FUNC_INFO << "User expired. Relogin";
                userReLogin();
            }
            else
                qDebug() << Q_FUNC_INFO << "Err" << obj["err"].toString();
            reply->deleteLater();
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
    }
    emit receivedPlayingList(songs);
    reply->deleteLater();
}

void DoubanFM::rateSong(const quint32& sid, const qint32 &channel, const bool toRate) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + ((_user) ? _user->user_id : QString())
            + QString("&expire=") + ((_user) ? _user->expire : QString())
            + QString("&token=") + ((_user) ? _user->token : QString())
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + (toRate? QString("&type=r"): QString("&type=u"));
    if (_managers[2] == nullptr) {
        _managers[2] = new QNetworkAccessManager(this);
        connect(_managers[2], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedRateSong(QNetworkReply*)));
    }

    auto request = QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args));
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    qDebug() << "Sending `rate` " << sid << " " << channel << " " << toRate;
    _managers[2]->get(request);
}

void DoubanFM::onReceivedRateSong(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occurred while sending `rate`: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QList<DoubanFMSong> songs;
    QJsonParseError parseerr;
    QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();

    if (parseerr.error == QJsonParseError::NoError) {
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

void DoubanFM::skipSong(const quint32 &sid, const qint32 &channel) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + ((_user) ? _user->user_id : QString())
            + QString("&expire=") + ((_user) ? _user->expire : QString())
            + QString("&token=") + ((_user) ? _user->token : QString())
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=s");
    if (_managers[3] == nullptr) {
        _managers[3] = new QNetworkAccessManager(this);
        connect(_managers[3], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedSkipSong(QNetworkReply*)));
    }

    auto request = QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args));
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    qDebug() << "Sending `skip` " << sid << " " << channel;
    _managers[3]->get(request);
}

void DoubanFM::onReceivedSkipSong(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occurred while sending `skip`: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJsonParseError parseerr;
    QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();

    if (parseerr.error == QJsonParseError::NoError) {
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

void DoubanFM::songEnd(const quint32& sid, const qint32 &channel) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + ((_user) ? _user->user_id : QString())
            + QString("&expire=") + ((_user) ? _user->expire : QString())
            + QString("&token=") + ((_user) ? _user->token : QString())
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=e");
    if (_managers[4] == nullptr) {
        _managers[4] = new QNetworkAccessManager(this);
        connect(_managers[4], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedCurrentEnd(QNetworkReply*)));
    }

    auto request = QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args));
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    qDebug() << "Sending `end` " << sid << " " << channel;
    _managers[4]->get(request);
}

void DoubanFM::onReceivedCurrentEnd(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occurred while sending `end`: " << reply->errorString();
        reply->deleteLater();
        return;
    }
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    if (all == "ok") emit receivedCurrentEnd(true);
    else emit receivedCurrentEnd(false);
}

void DoubanFM::byeSong(const quint32 &sid, const qint32 &channel) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + ((_user) ? _user->user_id : QString())
            + QString("&expire=") + ((_user) ? _user->expire : QString())
            + QString("&token=") + ((_user) ? _user->token : QString())
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=b");
    if (_managers[5] == nullptr) {
        _managers[5] = new QNetworkAccessManager(this);
        connect(_managers[5], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedByeSong(QNetworkReply*)));
    }

    auto request = QNetworkRequest(QUrl(DOUBAN_FM_API_ADDR + args));
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    qDebug() << "Sending `bye` " << sid << " " << channel;
    _managers[5]->get(request);
}

void DoubanFM::onReceivedByeSong(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occurred while sending `bye`: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJsonParseError parseerr;
    QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();

    if (parseerr.error == QJsonParseError::NoError) {
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

void DoubanFM::getChannels() {
    if (_managers[6] == nullptr) {
        _managers[6] = new QNetworkAccessManager(this);
        connect(_managers[6], SIGNAL(finished(QNetworkReply*)),
                this, SLOT(onReceivedChannels(QNetworkReply*)));
    }

    auto request = QNetworkRequest(QUrl(DOUBAN_FM_API_CHANNEL));
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(conf);

    qDebug() << "Getting channels from " << QUrl(DOUBAN_FM_API_CHANNEL);
    _managers[6]->get(request);
}

void DoubanFM::onReceivedChannels(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NetworkError::NoError) {
        qWarning() << "Error occured while getting channels: " << reply->errorString();
        reply->deleteLater();
        return;
    }

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());
    QList<DoubanChannel> channels;
    QJsonParseError parseerr;
    QVariant result = QJsonDocument::fromJson(all.toUtf8(), &parseerr).toVariant();

    if (parseerr.error == QJsonParseError::NoError) {

        QVariantMap obj = result.toMap();
        QVariantList chList = obj["channels"].toList();
        foreach(const QVariant& item, chList) {
            QVariantMap ch = item.toMap();
            DoubanChannel dc;
            dc.name = ch["name"].toString();
            dc.name_en = ch["name_en"].toString();
            dc.seq_id = ch["seq_id"].toInt();
            dc.channel_id = ch["channel_id"].toInt();
            dc.abbr_en = ch["abbr_en"].toString();
            channels.append(dc);
        }
    }

    std::sort(channels.begin(), channels.end(),
              [] (const DoubanChannel& a, const DoubanChannel& b) {
        return a.seq_id < b.seq_id;
    });
    DoubanChannel favourites;
    favourites.abbr_en = "fav";
    favourites.channel_id = -3;
    favourites.name = tr("My favourite");
    favourites.name_en = "favourites";
    favourites.seq_id = -3;
    channels.push_front(favourites);
    emit receivedChannels(channels);
    reply->deleteLater();
}

bool DoubanFM::hasLogin() {
    return !!_user;
}
