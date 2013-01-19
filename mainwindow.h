#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <QVector>
#include "doubanfmsong.h"
#include <phonon/MediaObject>
#include <phonon/SeekSlider>
#include <phonon/VolumeSlider>
#include <phonon/AudioOutput>
#include "doubanuser.h"
#include "networkmanager.h"

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
    NetworkManager manager;

    QList<DoubanFMSong> songs;
    QList<Phonon::MediaSource> mediaSources;

    Phonon::MediaObject *mediaObject;
    Phonon::AudioOutput *audioOutput;
    Phonon::SeekSlider *seekSlider;
    Phonon::VolumeSlider *volumeSilder;

private slots:
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void playTick(qint64 time);
    void sourceChanged(const Phonon::MediaSource &source);
    void on_pauseButton_clicked();
    void on_nextButton_clicked();
    void recvNewList(const QList<DoubanFMSong> &song);
    void recvPlayingList(const QList<DoubanFMSong> &song);
    void recvAlbumImage(const QByteArray &data);
};

static const QString DOUBAN_PLAYLIST_ADDR = "http://www.douban.com/j/app/radio/people";

#endif // MAINWINDOW_H
