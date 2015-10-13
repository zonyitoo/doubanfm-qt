#include "doubanplayer.h"

DoubanPlayer& DoubanPlayer::getInstance()
{
    static DoubanPlayer _instance(nullptr);
    return _instance;
}

DoubanPlayer::DoubanPlayer(QObject* parent)
    : QObject(parent)
    , player_(this, QMediaPlayer::StreamPlayback)
    , doubanfm(DoubanFM::getInstance())
    , _channel(-INT_MAX)
    , _volume(0)
    , _can_control(true)
    , bufplaylist(nullptr)
    , _kbps(64)
{
    connect(&doubanfm, &DoubanFM::receivedNewList,
        [this](const QList<DoubanFMSong>& rcvsongs) {
            this->songs_ = rcvsongs;
            qDebug() << "Received new playlist with" << rcvsongs.size()
                     << "songs";
            QMediaPlaylist* playlist = player_.playlist();
            if (playlist == nullptr) {
                playlist = new QMediaPlaylist(&player_);
            }
            playlist->clear();
            for (const DoubanFMSong& song : this->songs_) {
                playlist->addMedia(QUrl(song.url));
            }
            if (player_.playlist() == nullptr) {
                connect(playlist, SIGNAL(currentIndexChanged(int)), this,
                    SLOT(currentIndexChanged(int)));
                // FIXME: Crash on KDE4.9 libkdecore.so.5
                // Segmentation Fault by unknown reason
                player_.setPlaylist(playlist);
            }
            if (player_.state() != QMediaPlayer::PlayingState) {
                player_.play();
            }
            setCanControl(true);
        });

    connect(&doubanfm, &DoubanFM::receivedPlayingList,
        [this](const QList<DoubanFMSong>& rcvsongs) {
            qDebug() << "Received new playlist with" << rcvsongs.size()
                     << "songs";
            if (this->bufplaylist != nullptr) {
                delete this->bufplaylist;
            }
            this->bufplaylist = new QMediaPlaylist(&player_);
            bufsongs = rcvsongs;

            for (const DoubanFMSong& song : rcvsongs) {
                bufplaylist->addMedia(QUrl(song.url));
            }
            setCanControl(true);

            // if (player.state() != QMediaPlayer::PlayingState)
            //    player.play();
        });

    connect(&player_, &QMediaPlayer::positionChanged,
        [this](qint64 tick) { emit this->positionChanged(tick); });
    connect(&doubanfm, &DoubanFM::receivedByeSong, [this](bool suc) {
        emit this->receivedTrashSong(suc);
        setCanControl(true);
    });
    connect(&doubanfm, &DoubanFM::receivedRateSong, [this](bool suc) {
        int curIndex = player_.playlist()->currentIndex();
        songs_[curIndex].like = !songs_[curIndex].like;
        emit this->receivedRateSong(suc);
        setCanControl(true);
    });
    connect(&doubanfm, &DoubanFM::receivedSkipSong, [this](bool suc) {
        emit this->receivedSkipSong(suc);
        setCanControl(true);
    });
    connect(&player_, &QMediaPlayer::stateChanged,
        [this](QMediaPlayer::State s) { emit this->stateChanged(s); });
}

DoubanPlayer::~DoubanPlayer()
{
    player_.disconnect();
    if (player_.state() != QMediaPlayer::StoppedState) {
        player_.stop();
    }
}

void DoubanPlayer::currentIndexChanged(int position)
{
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
            player_.playlist()->deleteLater();
            player_.setPlaylist(bufplaylist);
            disconnect(player_.playlist(), SIGNAL(currentIndexChanged(int)), this,
                SLOT(currentIndexChanged(int)));
            connect(bufplaylist, SIGNAL(currentIndexChanged(int)), this,
                SLOT(currentIndexChanged(int)));
            songs_ = bufsongs;
            bufplaylist = nullptr;
            if (player_.state() != QMediaPlayer::PlayingState)
                player_.play();
            player_.playlist()->next();
        }
        qDebug() << "Deployed new playlist";
        return;
    }
    // Currently playing the last song in the list
    else if (position == songs_.size() - 1) {
        doubanfm.getPlayingList(_channel, songs_.back().sid, _kbps);
        this->setCanControl(false);
    }
    // Got update playlist
    else if (bufplaylist != nullptr) {
        player_.playlist()->deleteLater();
        player_.setPlaylist(bufplaylist);
        disconnect(player_.playlist(), SIGNAL(currentIndexChanged(int)), this,
            SLOT(currentIndexChanged(int)));
        connect(bufplaylist, SIGNAL(currentIndexChanged(int)), this,
            SLOT(currentIndexChanged(int)));
        songs_ = bufsongs;
        bufplaylist = nullptr;
        if (player_.state() != QMediaPlayer::PlayingState) {
            player_.play();
        }
        player_.playlist()->next();
        qDebug() << Q_FUNC_INFO << "Got updated playlist";
        return;
    }
    emit this->currentSongChanged(songs_[position]);

    qDebug() << "CurrentPlaying: ";
    qDebug() << "    artist: " << qPrintable(songs_[position].artist);
    qDebug() << "    title: " << qPrintable(songs_[position].title);
    qDebug() << "    album: " << qPrintable(songs_[position].albumtitle);
    qDebug() << "    publicTime: " << songs_[position].public_time;
    qDebug() << "    company: " << qPrintable(songs_[position].company);
    qDebug() << "    kbps: " << songs_[position].kbps;
    qDebug() << "    like: " << songs_[position].like;
    qDebug() << "    sid: " << songs_[position].sid;
    qDebug() << "    ssid: " << songs_[position].ssid;
    qDebug() << "    subType: " << songs_[position].subtype;
    qDebug() << "    url: " << songs_[position].url;
}

bool DoubanPlayer::canControl() const { return _can_control; }

void DoubanPlayer::setCanControl(bool can)
{
    _can_control = can;
    emit canControlChanged(can);
}

void DoubanPlayer::play()
{
    QPropertyAnimation* fadein = new QPropertyAnimation(&player_, "volume");
    fadein->setDuration(1000);
    fadein->setStartValue(player_.volume());
    player_.play();
    fadein->setEndValue(_volume);
    fadein->start(QPropertyAnimation::DeleteWhenStopped);
    emit playing();

    int elapsed = time(nullptr) - this->lastPausedTime;
    if (elapsed >= 30 * 60) {
        doubanfm.getPlayingList(_channel, this->currentSong().sid, _kbps);
        QTime pt(0, 0, 0, 0);
        pt = pt.addSecs(elapsed);
        this->setCanControl(false);
        qDebug() << "Have paused " << pt << ", getting a new playlist";
    }
}

void DoubanPlayer::pause()
{
    emit paused();
    this->lastPausedTime = time(nullptr);
    QPropertyAnimation* fadeout = new QPropertyAnimation(&player_, "volume");
    fadeout->setDuration(1000);
    fadeout->setStartValue(player_.volume());
    _volume = player_.volume();
    fadeout->setEndValue(0);
    connect(fadeout, &QPropertyAnimation::finished, [this]() { player_.pause(); });
    fadeout->start(QPropertyAnimation::DeleteWhenStopped);
}

const DoubanFMSong& DoubanPlayer::currentSong() const
{
    int sindex = -1;
    if (player_.playlist() == nullptr || (sindex = player_.playlist()->currentIndex()) < 0) {
        static DoubanFMSong loading;
        loading.title = "Loading";
        return loading;
    }
    return songs_[sindex];
}

qint64 DoubanPlayer::position() const { return player_.position(); }

void DoubanPlayer::next()
{
    if (player_.playlist() == nullptr)
        return;
    int sindex = player_.playlist()->currentIndex();
    if (sindex < 0) {
        doubanfm.getNewPlayList(this->_channel, _kbps);
        return;
    }
    doubanfm.skipSong(songs_[sindex].sid, _channel);
    player_.playlist()->next();
}

void DoubanPlayer::stop()
{
    this->player_.stop();
    emit stopped();
}

void DoubanPlayer::rateCurrentSong()
{
    if (player_.playlist() == nullptr)
        return;
    int sindex = player_.playlist()->currentIndex();
    doubanfm.rateSong(songs_[sindex].sid, _channel, true);
    setCanControl(false);
}

void DoubanPlayer::unrateCurrentSong()
{
    if (player_.playlist() == nullptr)
        return;
    int sindex = player_.playlist()->currentIndex();
    doubanfm.rateSong(songs_[sindex].sid, _channel, false);
    setCanControl(false);
}

void DoubanPlayer::trashCurrentSong()
{
    if (player_.playlist() == nullptr)
        return;
    int sindex = player_.playlist()->currentIndex();
    doubanfm.byeSong(songs_[sindex].sid, _channel);
    setCanControl(false);
}

void DoubanPlayer::setChannel(qint32 chanid)
{
    if (chanid == _channel)
        return;
    this->_channel = chanid;
    doubanfm.getNewPlayList(chanid, _kbps);
    setCanControl(false);
}

void DoubanPlayer::setVolume(int v)
{
    this->_volume = v;
    this->player_.setVolume(v);
}

qint32 DoubanPlayer::channel() const { return this->_channel; }

int DoubanPlayer::volume() const { return player_.volume(); }

qint64 DoubanPlayer::duration() const { return player_.duration(); }

QMediaPlayer::State DoubanPlayer::state() const { return player_.state(); }

void DoubanPlayer::setKbps(qint32 kbps) { this->_kbps = kbps; }

qint32 DoubanPlayer::kbps() const { return this->_kbps; }
