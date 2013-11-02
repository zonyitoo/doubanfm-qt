#ifndef DOUBANPLAYER_H
#define DOUBANPLAYER_H

#include <QObject>
#include <doubanfm.h>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class DoubanPlayer : public QObject
{
    Q_OBJECT
public:
    static DoubanPlayer *getInstance();

    qint64 position() const;
    qint64 duration() const;

    DoubanFMSong currentSong() const;
    qint32 channel() const;
    int volume() const;
    QMediaPlayer::State state() const;

signals:

    void currentSongChanged(const DoubanFMSong& song);
    void positionChanged(qint64);

    void receivedSkipSong(bool);
    void receivedRateSong(bool);
    void receivedTrashSong(bool);

    void stateChanged(QMediaPlayer::State);

public slots:
    void next();
    void pause();
    void play();
    void stop();

    void rateCurrentSong();
    void unrateCurrentSong();
    void trashCurrentSong();

    void setChannel(qint32);
    void setVolume(int);

private:
    explicit DoubanPlayer(QObject *parent = 0);

    QList<DoubanFMSong> songs;
    QMediaPlayer player;
    DoubanFM *doubanfm;
    qint32 _channel;
    qint32 _volume;
};

#endif // DOUBANPLAYER_H
