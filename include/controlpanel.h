#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include "douban_types.h"
#include "douban.h"
#include <phonon/MediaObject>
#include <phonon/SeekSlider>
#include <phonon/VolumeSlider>
#include <phonon/AudioOutput>

namespace Ui {
class ControlPanel;
}

class ControlPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit ControlPanel(QWidget *parent = 0);
    ~ControlPanel();

    int getVisibleHeight();
    qint32 getCurrentChannel();

    void play();
    void pause();

signals:
    void gotAlbumImage(const QString& url);
    void userButtonClicked();

private slots:
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void playTick(qint64 time);
    void sourceChanged(const Phonon::MediaSource &source);

    void on_pauseButton_clicked();
    void on_nextButton_clicked();
    void on_trashButton_clicked();
    void on_heartButton_clicked();
    void on_volumeSlider_sliderMoved(int position);
    void on_userNameButton_clicked();
    void on_volumeButton_clicked();

    void recvNewList(const QList<DoubanFMSong> &song);
    void recvPlayingList(const QList<DoubanFMSong> &song);
    void recvRateSong(const bool succeed);
    void recvUserLoginSucceed(DoubanUser user);

    void onChannelChanged(qint32 channel);

private:
    Ui::ControlPanel *ui;

    Douban *_douban;

    QList<DoubanFMSong> songs;
    QList<Phonon::MediaSource> mediaSources;

    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;

    qint32 _channel;
    QList<DoubanChannel> channels;

    void loadBackupData();
    void saveBackupData();

    void freeze();
    void unfreeze();

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    void mousePressEvent(QMouseEvent *event);
};

#endif // CONTROLPANEL_H
