#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QVector>
#include <phonon/MediaObject>
#include <phonon/SeekSlider>
#include <phonon/VolumeSlider>
#include <phonon/AudioOutput>
#include "douban.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *_networkmgr;
    Douban *_douban;

    QList<DoubanFMSong> songs;
    QList<Phonon::MediaSource> mediaSources;

    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    Phonon::SeekSlider *seekSlider;
    Phonon::VolumeSlider *volumeSilder;

    qint32 _channel;

    void getImage(const QString &url);
    void recvAlbumImage(const QByteArray &data);
    void loadBackupData(const QString& filename);
    void saveBackupData(const QString& filename);

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
    void on_channelComboBox_currentIndexChanged(int index);
    void recvRateSong(const bool succeed);
};

static const QString DOUBAN_PLAYLIST_ADDR = "http://www.douban.com/j/app/radio/people";

#endif // MAINWINDOW_H
