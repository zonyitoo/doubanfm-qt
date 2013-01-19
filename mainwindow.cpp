#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qjson/parser.h>
#include <QMessageBox>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), manager(this) {

    ui->setupUi(this);
    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(this);
    seekSlider = new Phonon::SeekSlider(this);
    volumeSilder = new Phonon::VolumeSlider(this);

    ui->controlLayout->addWidget(seekSlider);
    QHBoxLayout *volumeControlLayout = new QHBoxLayout(this);
    volumeControlLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    volumeControlLayout->addWidget(volumeSilder);
    ui->controlLayout->addItem(volumeControlLayout);

    seekSlider->setMediaObject(mediaObject);
    volumeSilder->setAudioOutput(audioOutput);
    Phonon::createPath(mediaObject, audioOutput);

    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(playTick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(sourceChanged(Phonon::MediaSource)));
    //connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish()));

    connect(&manager, SIGNAL(receivedNewList(QList<DoubanFMSong>)),
            this, SLOT(recvNewList(QList<DoubanFMSong>)));
    connect(&manager, SIGNAL(receivedPlayingList(QList<DoubanFMSong>)),
            this, SLOT(recvPlayingList(QList<DoubanFMSong>)));
    connect(&manager, SIGNAL(receivedImage(QByteArray)),
            this, SLOT(recvAlbumImage(QByteArray)));

    manager.getNewList(2);
}

MainWindow::~MainWindow() {
    delete ui;
    delete mediaObject;
    delete audioOutput;
    delete seekSlider;
}

void MainWindow::playTick(qint64 time) {
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
    ui->currentTick->setText(displayTime.toString("mm:ss"));
}

void MainWindow::sourceChanged(const Phonon::MediaSource& source) {
    int index = mediaSources.indexOf(source);
    qDebug() << Q_FUNC_INFO << "Current Index =" << index;


    ui->artistName->setText(songs[index].artist);
    ui->albumName->setText(songs[index].albumtitle);
    ui->songName->setText("<font color='green'>" + songs[index].title + "</font>");

    manager.getImage(songs[index].picture);
}

static bool pause_state = false;
void MainWindow::stateChanged(Phonon::State newState, Phonon::State oldState) {
    switch (newState) {
    case Phonon::ErrorState:
        qDebug() << Q_FUNC_INFO << "Phonon::ErrorState";
        if (mediaObject->errorType() == Phonon::FatalError) {
            QMessageBox::warning(this, tr("Fatal Error"),
            mediaObject->errorString());
        }
        else {
            QMessageBox::warning(this, tr("Error"),
            mediaObject->errorString());
        }
        break;
    case Phonon::PlayingState:
        qDebug() << Q_FUNC_INFO << "Phonon::PlayingState";
        pause_state = false;
        break;
    case Phonon::StoppedState:
        qDebug() << Q_FUNC_INFO << "Phonon::StoppedState";
        if (mediaSources.indexOf(mediaObject->currentSource()) == mediaSources.size() - 1)
            manager.getNewList(2);
        break;
    case Phonon::PausedState:
        qDebug() << Q_FUNC_INFO << "Phonon::PausedState";
        pause_state = true;
        break;
    default:
        break;
    }
}

void MainWindow::on_pauseButton_clicked() {
    if (pause_state) {
        emit mediaObject->play();
    }
    else {
        emit mediaObject->pause();
    }
}

void MainWindow::on_nextButton_clicked() {
    mediaObject->seek(mediaObject->totalTime());
}

void MainWindow::recvNewList(const QList<DoubanFMSong> &song) {
    qDebug() << Q_FUNC_INFO;
    this->songs = song;
    mediaSources.clear();
    foreach(DoubanFMSong s, song) {
        mediaSources.append(s.url);
        qDebug() << Q_FUNC_INFO << s.url;
    }
    mediaObject->clear();
    mediaObject->setQueue(mediaSources);
    mediaObject->play();
}

void MainWindow::recvPlayingList(const QList<DoubanFMSong> &song) {
    qDebug() << Q_FUNC_INFO;
    this->songs = song;
    mediaSources.clear();
    foreach(DoubanFMSong s, song) {
        mediaSources.append(s.url);
        qDebug() << Q_FUNC_INFO << s.url;
    }
    mediaObject->clear();
    mediaObject->setQueue(mediaSources);
    mediaObject->play();
}

void MainWindow::recvAlbumImage(const QByteArray &data) {
    QPixmap pixmap;
    pixmap.loadFromData(data);
    int w = ui->albumImage->width();
    int h = ui->albumImage->height();
    ui->albumImage->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatioByExpanding));
}
