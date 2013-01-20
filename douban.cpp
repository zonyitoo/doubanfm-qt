#include "douban.h"
#include <qjson/parser.h>
#include <QtNetwork/QtNetwork>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

static Douban *_INSTANCE = NULL;
static const QString DOUBAN_FM_API_ADDR = "http://www.douban.com/j/app/radio/people";
static const QString DOUBAN_FM_API_CHANNEL = "http://www.douban.com/j/app/radio/channels";

Douban::Douban(QObject *parent) : QObject(parent) {
    for (int i = 0; i < 7; ++ i)
        _managers[i] = new QNetworkAccessManager(this);

    connect(_managers[1], SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReceivedNewList(QNetworkReply*)));
    connect(_managers[2], SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReceivedRateSong(QNetworkReply*)));
    connect(_managers[3], SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReceivedSkipSong(QNetworkReply*)));
    connect(_managers[4], SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReceivedCurrentEnd(QNetworkReply*)));
    connect(_managers[5], SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReceivedByeSong(QNetworkReply*)));
    connect(_managers[6], SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReceivedChannels(QNetworkReply*)));
}


Douban::~Douban() {
    for (int i = 0; i < 7; ++ i)
        delete _managers[i];
}

Douban* Douban::getInstance(const DoubanUser &user) {
    if (_INSTANCE == NULL) {
        _INSTANCE = new Douban();
    }
    _INSTANCE->_user = user;
    return _INSTANCE;
}

DoubanUser Douban::userLogin(const QString& name, const QString& password) {

}

void Douban::userLogout() {

}

void Douban::setUser(const DoubanUser& user) {
    _user = user;
}

void Douban::loadUserFromXML(const QString& xmlfilename) {
    QFile file(xmlfilename);
    QDomDocument doc;
    if (!file.open(QIODevice::ReadOnly)) return;
    if (doc.setContent(&file)) {
        QDomElement user = doc.documentElement();
        QString user_id = user.attribute("user_id");

        QString expire, token;

        QDomNodeList list = user.childNodes();
        for (int i = 0; i < list.size(); ++ i) {
            QDomNode node = list.at(i);
            if (node.nodeName() == "expire") {
                expire = node.toElement().text();
            }
            else if (node.nodeName() == "token") {
                token = node.toElement().text();
            }
        }

        if (user_id.isEmpty() || expire.isEmpty() || token.isEmpty()) return;

        _user.user_id = user_id;
        _user.expire = expire;
        _user.token = token;

        qDebug() << Q_FUNC_INFO << _user.user_id << _user.expire << _user.token;
    }
    file.close();
}

void Douban::saveUserToXML(const QString& xmlfilename) {
    QFile file(xmlfilename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");

    QDomDocument doc;

    QDomProcessingInstruction instruction =
            doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement user = doc.createElement("user");
    user.setAttribute("user_id", _user.user_id);
    QDomElement expire = doc.createElement("expire");
    QDomText expire_t = doc.createTextNode(_user.expire);
    expire.appendChild(expire_t);
    QDomElement token = doc.createElement("token");
    QDomText token_t = doc.createTextNode(_user.token);
    token.appendChild(token_t);
    user.appendChild(expire);
    user.appendChild(token);
    doc.appendChild(user);
    doc.save(out, 4, QDomNode::EncodingFromTextStream);

    file.close();
}

void Douban::getNewPlayList(const quint32& channel) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + _user.user_id
            + QString("&expire=") + _user.expire
            + QString("&token=") + _user.token
            + QString("&sid=&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + QString("&type=n");

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
        QVariantList songList = obj["song"].toList();
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

void Douban::rateSong(const quint32& sid, const quint32 &channel, const bool toRate) {
    QString args = QString("?app_name=radio_desktop_win&version=100")
            + QString("&user_id=") + _user.user_id
            + QString("&expire=") + _user.expire
            + QString("&token=") + _user.token
            + QString("&sid=") + QString::number(sid)
            + QString("&h=")
            + QString("&channel=") + QString::number(channel, 10)
            + (toRate? QString("&type=r"): QString("&type=u"));
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
            emit receivedByeSong(false);
        }
        else {
            emit receivedByeSong(true);
        }
    }
    reply->deleteLater();
}

void Douban::getChannels() {
    _managers[6]->get(QNetworkRequest(QUrl(DOUBAN_FM_API_CHANNEL)));
}

void Douban::onReceivedChannels(QNetworkReply *reply) {
    qDebug() << Q_FUNC_INFO;
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    QList<DoubanChannel> channels;

    if (ok) {
        QVariantMap obj = result.toMap();
        QVariantList chList = obj["channels"].toList();
        foreach(QVariant item, chList) {
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

    emit receivedChannels(channels);
    reply->deleteLater();
}
