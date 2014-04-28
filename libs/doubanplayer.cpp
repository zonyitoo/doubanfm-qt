#include "doubanplayer.h"

DoubanPlayer& DoubanPlayer::getInstance() {
    static DoubanPlayer _instance(nullptr);
    return _instance;
}

DoubanPlayer::DoubanPlayer(QObject *parent) :
    QObject(parent),
    doubanfm(DoubanFM::getInstance()),
    _channel(-INT_MAX), _volume(0), _can_control(true),
    bufplaylist(nullptr), _kbps(64)
{
    connect(&doubanfm, &DoubanFM::receivedNewList, [this] (const QList<DoubanFMSong>& rcvsongs) {
        this->songs = rcvsongs;
        qDebug() << "Received new playlist with" << rcvsongs.size() << "songs";
        QMediaPlaylist *playlist = player.playlist();
        if (playlist == nullptr) playlist = new QMediaPlaylist(&player);
        playlist->clear();
        for (const DoubanFMSong& song : this->songs) {
            playlist->addMedia(QUrl(song.url));
        }
        if (player.playlist() == nullptr) {
            connect(playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
            // FIXME: Crash on KDE4.9 libkdecore.so.5
            // Segmentation Fault by unknown reason
            player.setPlaylist(playlist);
        }
        if (player.state() != QMediaPlayer::PlayingState)
            player.play();
        setCanControl(true);
    });

    connect(&doubanfm, &DoubanFM::receivedPlayingList, [this] (const QList<DoubanFMSong>& rcvsongs) {
        qDebug() << "Received new playlist with" << rcvsongs.size() << "songs";
        if (this->bufplaylist != nullptr) delete this->bufplaylist;
        this->bufplaylist = new QMediaPlaylist(&player);
        bufsongs = rcvsongs;

        for (const DoubanFMSong& song : rcvsongs) {
            bufplaylist->addMedia(QUrl(song.url));
        }
        setCanControl(true);

        //if (player.state() != QMediaPlayer::PlayingState)
        //    player.play();
    });

    connect(&player, &QMediaPlayer::positionChanged, [this] (qint64 tick) { emit this->positionChanged(tick); });
    connect(&doubanfm, &DoubanFM::receivedByeSong, [this] (bool suc) {
        emit this->receivedTrashSong(suc);
        setCanControl(true);
    });
    connect(&doubanfm, &DoubanFM::receivedRateSong, [this] (bool suc) {
        int curIndex = player.playlist()->currentIndex();
        songs[curIndex].like = !songs[curIndex].like;
        emit this->receivedRateSong(suc);
        setCanControl(true);
    });
    connect(&doubanfm, &DoubanFM::receivedSkipSong, [this] (bool suc) {
        emit this->receivedSkipSong(suc);
        setCanControl(true);
    });
    connect(&player, &QMediaPlayer::stateChanged, [this] (QMediaPlayer::State s) { emit this->stateChanged(s); });
}

DoubanPlayer::~DoubanPlayer() {
    player.disconnect();
    if (player.state() != QMediaPlayer::StoppedState) {
        player.stop();
    }
}

void DoubanPlayer::currentIndexChanged(int position) {
    /*if (position < 0) {
        if (songs.size() > 0)
            doubanfm->getPlayingList(channel, songs.back().sid);
        else
            doubanfm->(channel);
        return;
    }*/
    // Jump out of playlist
    if (position < 0) {
        if (bufplaylist == nullptr) {
            doubanfm.getNewPlayList(_channel, _kbps);
            setCanControl(false);
        }
        else {
            player.playlist()->deleteLater();
            player.setPlaylist(bufplaylist);
            disconnect(player.playlist(), SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
            connect(bufplaylist, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
            songs = bufsongs;
            bufplaylist = nullptr;
            if (player.state() != QMediaPlayer::PlayingState)
                player.play();
            player.playlist()->next();
        }
        qDebug() << Q_FUNC_INFO << "Deployed new playlist";
        return;
    }
    // Currently playing the last song in the list
    else if (position == songs.size() - 1) {
        doubanfm.getPlayingList(_channel, songs.back().sid, _kbps);
        this->setCanControl(false);
    }
    // Got update playlist
    else if (bufplaylist != nullptr) {
        player.playlist()->deleteLater();
        player.setPlaylist(bufplaylist);
        disconnect(player.playlist(), SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        connect(bufplaylist, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        songs = bufsongs;
        bufplaylist = nullptr;
        if (player.state() != QMediaPlayer::PlayingState)
            player.play();
        player.playlist()->next();
        qDebug() << Q_FUNC_INFO << "Got updated playlist";
        return;
    }
    emit this->currentSongChanged(songs[position]);

    qDebug() << "CurrentPlaying: ";
    qDebug() << "    artist: " << songs[position].artist;
    qDebug() << "    title: " << songs[position].title;
    qDebug() << "    album: " << songs[position].albumtitle;
    qDebug() << "    publicTime: " << songs[position].public_time;
    qDebug() << "    company: " << songs[position].company;
    qDebug() << "    kbps: " << songs[position].kbps;
    qDebug() << "    like: " << songs[position].like;
    qDebug() << "    sid: " << songs[position].sid;
    qDebug() << "    subType: " << songs[position].subtype;
}

bool DoubanPlayer::canControl() const {
    return _can_control;
}

void DoubanPlayer::setCanControl(bool can) {
    _can_control = can;
    emit canControlChanged(can);
}

void DoubanPlayer::play() {
    QPropertyAnimation *fadein = new QPropertyAnimation(&player, "volume");
    fadein->setDuration(1000);
    fadein->setStartValue(player.volume());
    player.play();
    fadein->setEndValue(_volume);
    fadein->start(QPropertyAnimation::DeleteWhenStopped);
    emit playing();

    int elapsed = time(nullptr) - this->lastPausedTime;
    if (elapsed >= 30 * 60) {
        doubanfm.getPlayingList(_channel, this->currentSong().sid, _kbps);
        QTime pt(0, 0, 0, 0);
        pt.addSecs(elapsed);
        this->setCanControl(false);
        qDebug() << "Have paused " << pt <<  ", getting a new playlist";
    }
}

void DoubanPlayer::pause() {
    emit paused();
    this->lastPausedTime = time(nullptr);
    QPropertyAnimation *fadeout = new QPropertyAnimation(&player, "volume");
    fadeout->setDuration(1000);
    fadeout->setStartValue(player.volume());
    _volume = player.volume();
    fadeout->setEndValue(0);
    connect(fadeout, &QPropertyAnimation::finished, [this] () {
        player.pause();
    });
    fadeout->start(QPropertyAnimation::DeleteWhenStopped);
}

const DoubanFMSong& DoubanPlayer::currentSong() const {
    int sindex = -1;
    if (player.playlist() == nullptr
            || (sindex = player.playlist()->currentIndex()) < 0) {
        static DoubanFMSong loading;
        loading.title = "Loading";
        return loading;
    }
    return songs[sindex];
}

qint64 DoubanPlayer::position() const {
    return player.position();
}

void DoubanPlayer::next() {
    if (player.playlist() == nullptr) return;
    int sindex = player.playlist()->currentIndex();
    if (sindex < 0) {
        doubanfm.getNewPlayList(this->_channel, _kbps);
        return;
    }
    doubanfm.skipSong(songs[sindex].sid, _channel);
    player.playlist()->next();
}

void DoubanPlayer::stop() {
    this->player.stop();
    emit stopped();
}

void DoubanPlayer::rateCurrentSong() {
    if (player.playlist() == nullptr) return;
    int sindex = player.playlist()->currentIndex();
    doubanfm.rateSong(songs[sindex].sid, _channel, true);
    setCanControl(false);
}

void DoubanPlayer::unrateCurrentSong() {
    if (player.playlist() == nullptr) return;
    int sindex = player.playlist()->currentIndex();
    doubanfm.rateSong(songs[sindex].sid, _channel, false);
    setCanControl(false);
}

void DoubanPlayer::trashCurrentSong() {
    if (player.playlist() == nullptr) return;
    int sindex = player.playlist()->currentIndex();
    doubanfm.byeSong(songs[sindex].sid, _channel);
    setCanControl(false);
}

void DoubanPlayer::setChannel(qint32 chanid) {
    if (chanid == _channel) return;
    this->_channel = chanid;
    doubanfm.getNewPlayList(chanid, _kbps);
    setCanControl(false);
}

void DoubanPlayer::setVolume(int v) {
    this->_volume = v;
    this->player.setVolume(v);
}

qint32 DoubanPlayer::channel() const {
    return this->_channel;
}

int DoubanPlayer::volume() const {
    return player.volume();
}

qint64 DoubanPlayer::duration() const {
    return player.duration();
}

QMediaPlayer::State DoubanPlayer::state() const {
    return player.state();
}

void DoubanPlayer::setKbps(qint32 kbps) {
    this->_kbps = kbps;
}

qint32 DoubanPlayer::kbps() const {
    return this->_kbps;
}
