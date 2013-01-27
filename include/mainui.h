#ifndef MAINUI_H
#define MAINUI_H

#include <QWidget>
#include <QtNetwork/QtNetwork>
#include <QVector>
#include <phonon/MediaObject>
#include <phonon/SeekSlider>
#include <phonon/VolumeSlider>
#include <phonon/AudioOutput>
#include "douban.h"
#include "channelbutton.h"

namespace Ui {
class MainUI;
}

class MainUI : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainUI(QWidget *parent = 0);
    ~MainUI();
    
private slots:
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void playTick(qint64 time);
    void sourceChanged(const Phonon::MediaSource &source);
    void on_pauseButton_clicked();
    void on_nextButton_clicked();
    void recvNewList(const QList<DoubanFMSong> &song);
    void recvPlayingList(const QList<DoubanFMSong> &song);
    void on_trashButton_clicked();
    void on_heartButton_clicked();
    void onReceivedImage(QNetworkReply *reply);
    void onReceivedChannels(const QList<DoubanChannel>& channels);
    void recvRateSong(const bool succeed);
    void recvUserLogin(DoubanUser *user);
    void recvUserLogoff();
    void recvUserLoginFailed(const QString &errmsg);
    void on_userNameButton_clicked();
    void on_loginButton_clicked();
    void on_channelButton_clicked();
    void on_chooseChannel(const ChannelButton* button);

private:
    Ui::MainUI *ui;

    QNetworkAccessManager *_networkmgr;
    Douban *_douban;

    QList<DoubanFMSong> songs;
    QList<Phonon::MediaSource> mediaSources;

    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;

    qint32 _channel;
    QList<DoubanChannel> channels;

    void getImage(const QString &url);
    void recvAlbumImage(const QByteArray &data);
    void loadBackupData();
    void saveBackupData();

    void freeze();
    void unfreeze();

    QRegExp emailRegExp;
};

#endif // MAINUI_H
