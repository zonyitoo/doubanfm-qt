#include "doubanmprisplugin.h"
#include <QDBusConnection>
#include "mprisadapter.h"
#include "mprisplayeradapter.h"
#include <QApplication>
#include <QWidget>

static const QString PLAYER_SERVICE_NAME = "org.mpris.MediaPlayer2.doubanfm";
static const QString PLAYER_OBJECT_PATH = "/org/mpris/MediaPlayer2";

static bool DBUS_NOTIFY_PROPERTIES_CHANGED(QString iface, QVariantMap changed, QStringList invalidated = QStringList()) {
    auto dbus_msg = QDBusMessage::createSignal(PLAYER_OBJECT_PATH,
                                               "org.freedesktop.DBus.Properties",
                                               "PropertiesChanged");
    dbus_msg << iface << changed << invalidated;
    return QDBusConnection::sessionBus().send(dbus_msg);
}

DoubanMprisPlugin::DoubanMprisPlugin(QObject *parent) :
    DoubanFMPlugin(parent)
{
    QDBusConnection con = QDBusConnection::sessionBus();
    //con.registerService("org.mpris.MediaPlayer2");
    con.registerService(PLAYER_SERVICE_NAME);
    new MprisAdapter(this);
    new MprisPlayerAdapter(this);
    con.registerObject(PLAYER_OBJECT_PATH, this);

    connect(&player, &DoubanPlayer::stateChanged, [=] (QMediaPlayer::State) {
        QVariantMap changedMap;
        changedMap.insert("PlaybackStatus", this->PlaybackStatus());
        DBUS_NOTIFY_PROPERTIES_CHANGED("org.mpris.MediaPlayer2.Player", changedMap);
    });

    connect(&player, &DoubanPlayer::currentSongChanged, [=] (const DoubanFMSong& song) {
        QVariantMap map;
        map.insert("mpris:length", song.length);
        map.insert("xesam:album", song.albumtitle);
        map.insert("mpris:artUrl", song.picture);
        map.insert("xesam:title", song.title);
        map.insert("xesam:artist", song.artist);
        map.insert("mpris:trackid", song.sid);
        QVariantList artistlist;
        artistlist.append(QVariant(song.artist));
        map.insert("xesam:albumArtist", artistlist);
        map.insert("xesam:url", song.url);
        QVariantMap changedMap;
        changedMap.insert("Metadata", map);
        DBUS_NOTIFY_PROPERTIES_CHANGED("org.mpris.MediaPlayer2.Player", changedMap);
    });
}

bool DoubanMprisPlugin::CanControl() const {
    return true;
}
bool DoubanMprisPlugin::CanGoNext() const {
    return true;
}
bool DoubanMprisPlugin::CanGoPrevious() const {
    return false;
}
bool DoubanMprisPlugin::CanPause() const {
    return true;
}
bool DoubanMprisPlugin::CanSeek() const {
    return false;
}
qlonglong DoubanMprisPlugin::Position() const {
    return this->player.position();
}

bool DoubanMprisPlugin::CanQuit() const {
    return true;
}
bool DoubanMprisPlugin::CanRaise() const {
    return true;
}
QString DoubanMprisPlugin::DesktopEntry() const {
    return "QDoubanFM";
}
QString DoubanMprisPlugin::Identity() const {
    return "QDoubanFM";
}

QVariantMap DoubanMprisPlugin::Metadata() const {
    QVariantMap map;
    DoubanFMSong song = player.currentSong();
    map.insert("mpris:length", song.length);
    map.insert("xesam:album", song.albumtitle);
    map.insert("mpris:artUrl", song.picture);
    map.insert("xesam:title", song.title);
    map.insert("xesam:artist", song.artist);
    map.insert("mpris:trackid", song.sid);
    QVariantList artistlist;
    artistlist.append(QVariant(song.artist));
    map.insert("xesam:albumArtist", artistlist);
    map.insert("xesam:url", song.url);
    return map;
}

QString DoubanMprisPlugin::PlaybackStatus() const {
    auto status = player.state();
    switch (status) {
    case QMediaPlayer::PlayingState:
        return "Playing";
    case QMediaPlayer::PausedState:
        return "Paused";
    default:
        return "Stopped";
    }
}

void DoubanMprisPlugin::Next() {
    this->player.next();
}

void DoubanMprisPlugin::Pause() {
    this->player.pause();
}

void DoubanMprisPlugin::Play() {
    this->player.play();
}

void DoubanMprisPlugin::PlayPause() {
    switch (player.state()) {
    case QMediaPlayer::PlayingState:
        player.pause();
        break;
    default:
        player.play();
    }
}

void DoubanMprisPlugin::Stop() {
    this->player.stop();
}

void DoubanMprisPlugin::Quit() {
    qApp->quit();
}

void DoubanMprisPlugin::Raise() {
    auto widgetlist = QApplication::topLevelWidgets();
    for (QWidget *widget : widgetlist) {
        if (!widget->parent()) {
            if (widget->isHidden()) widget->show();
            else widget->activateWindow();
            break;
        }
    }
}
