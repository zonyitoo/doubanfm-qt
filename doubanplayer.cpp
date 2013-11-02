#include "doubanplayer.h"

DoubanPlayer *DoubanPlayer::getInstance() {
    static DoubanPlayer *_instance = nullptr;
    if (_instance == nullptr)
        _instance = new DoubanPlayer(nullptr);
    return _instance;
}

DoubanPlayer::DoubanPlayer(QObject *parent) :
    QObject(parent),
    doubanfm(DoubanFM::getInstance()),
    _channel(-3), _volume(0)
{
    player.setPlaylist(new QMediaPlaylist(&player));
    connect(player.playlist(), &QMediaPlaylist::currentIndexChanged, [=] (int position) {
        /*if (position < 0) {
            if (songs.size() > 0)
                doubanfm->getPlayingList(channel, songs.back().sid);
            else
                doubanfm->getNewPlayList(channel);
            return;
        }*/
        if (position < 0) {
            doubanfm->getNewPlayList(_channel);
            return;
        }
        else if (position == songs.size() - 1) {
            doubanfm->getPlayingList(_channel, songs.back().sid);
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

    });

    connect(doubanfm, &DoubanFM::receivedNewList, [this] (const QList<DoubanFMSong>& rcvsongs) {
        this->songs = rcvsongs;
        qDebug() << "Received new playlist with" << rcvsongs.size() << "songs";
        QMediaPlaylist *playlist = player.playlist();
        playlist->clear();
        for (const DoubanFMSong& song : this->songs) {
            playlist->addMedia(QUrl(song.url));
        }
        if (player.state() != QMediaPlayer::PlayingState)
            player.play();
    });

    connect(doubanfm, &DoubanFM::receivedPlayingList, [this] (const QList<DoubanFMSong>& rcvsongs) {
        qDebug() << "Received new playlist with" << rcvsongs.size() << "songs";
        QMediaPlaylist *playlist = player.playlist();
        int curIndex = playlist->currentIndex();
        if (curIndex < 0) {
            playlist->clear();
            this->songs = rcvsongs;
        }
        else {
            DoubanFMSong curSong = songs[curIndex];
            if (curIndex != 0) playlist->removeMedia(0, curIndex - 1);
            if (curIndex != songs.size() - 1) playlist->removeMedia(curIndex + 1, songs.size() - 1);
            songs.clear();
            songs = rcvsongs;
            songs.insert(0, curSong);
        }

        for (const DoubanFMSong& song : this->songs) {
            playlist->addMedia(QUrl(song.url));
        }

        //if (player.state() != QMediaPlayer::PlayingState)
        //    player.play();
    });

    connect(&player, &QMediaPlayer::positionChanged, [this] (qint64 tick) { emit this->positionChanged(tick); });
    connect(doubanfm, &DoubanFM::receivedByeSong, [this] (bool suc) {
        emit this->receivedTrashSong(suc);
    });
    connect(doubanfm, &DoubanFM::receivedRateSong, [this] (bool suc) {
        int curIndex = player.playlist()->currentIndex();
        songs[curIndex].like = !songs[curIndex].like;
        emit this->receivedRateSong(suc);
    });
    connect(doubanfm, &DoubanFM::receivedSkipSong, [this] (bool suc) { emit this->receivedSkipSong(suc); });
    connect(&player, &QMediaPlayer::stateChanged, [this] (QMediaPlayer::State s) { emit this->stateChanged(s); });
}

void DoubanPlayer::play() {
    QPropertyAnimation *fadein = new QPropertyAnimation(&player, "volume");
    fadein->setDuration(1000);
    fadein->setStartValue(player.volume());
    player.play();
    fadein->setEndValue(_volume);
    fadein->start(QPropertyAnimation::DeleteWhenStopped);
}

void DoubanPlayer::pause() {
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

DoubanFMSong DoubanPlayer::currentSong() const {
    int sindex = player.playlist()->currentIndex();
    if (sindex < 0) return DoubanFMSong();
    return songs[sindex];
}

qint64 DoubanPlayer::position() const {
    return player.position();
}

void DoubanPlayer::next() {
    int sindex = player.playlist()->currentIndex();
    doubanfm->skipSong(songs[sindex].sid, _channel);
    player.playlist()->next();
}

void DoubanPlayer::stop() {
    this->player.stop();
}

void DoubanPlayer::rateCurrentSong() {
    int sindex = player.playlist()->currentIndex();
    doubanfm->rateSong(songs[sindex].sid, _channel, true);
}

void DoubanPlayer::unrateCurrentSong() {
    int sindex = player.playlist()->currentIndex();
    doubanfm->rateSong(songs[sindex].sid, _channel, false);
}

void DoubanPlayer::trashCurrentSong() {
    int sindex = player.playlist()->currentIndex();
    doubanfm->byeSong(songs[sindex].sid, _channel);
}

void DoubanPlayer::setChannel(qint32 chanid) {
    if (chanid == _channel) return;
    this->_channel = chanid;
    doubanfm->getNewPlayList(chanid);
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
