#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qjson/parser.h>
#include <QMessageBox>
#include <QPixmap>
#include <QMovie>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);
    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(this);
    _douban = Douban::getInstance();
    _networkmgr = new QNetworkAccessManager(this);

    ui->seekSlider->setMediaObject(mediaObject);
    ui->seekSlider->setEnabled(false);
    ui->volumeSlider->setAudioOutput(audioOutput);
    Phonon::createPath(mediaObject, audioOutput);

    ui->trashButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_TrashIcon)));
    ui->pauseButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_MediaPause)));
    ui->nextButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_MediaSkipForward)));

    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(playTick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(sourceChanged(Phonon::MediaSource)));

    connect(_networkmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReceivedImage(QNetworkReply*)));

    connect(_douban, SIGNAL(receivedNewList(QList<DoubanFMSong>)),
            this, SLOT(recvNewList(QList<DoubanFMSong>)));
    connect(_douban, SIGNAL(receivedPlayingList(QList<DoubanFMSong>)),
            this, SLOT(recvPlayingList(QList<DoubanFMSong>)));
    connect(_douban, SIGNAL(receivedChannels(QList<DoubanChannel>)),
            this, SLOT(onReceivedChannels(QList<DoubanChannel>)));
    connect(_douban, SIGNAL(receivedRateSong(bool)), this, SLOT(recvRateSong(bool)));
    connect(_douban, SIGNAL(loginSucceed(DoubanUser*)),
            this, SLOT(recvUserLogin(DoubanUser*)));
    connect(_douban, SIGNAL(logoffSucceed()), this, SLOT(recvUserLogoff()));

    _channel = 1;
    this->loadBackupData();
    _douban->getChannels();
    _douban->getNewPlayList(_channel);

    DoubanLoginDialog *logindialog = new DoubanLoginDialog(this);
    _douban->setLoginDialog(logindialog);

    if (_douban->hasLogin()) {
        ui->userNameButton->setText(_douban->getUser().user_name);
    }
}

void MainWindow::loadBackupData() {
    QSettings settings("QDoubanFM", "DoubanFM");

    _channel = settings.value("channel", 0).toInt();
    audioOutput->setVolume(settings.value("volume", 0.5).toDouble());
    QVariantMap user = settings.value("user").toMap();

    if (!user.empty()) {
        DoubanUser nuser;
        nuser.email = user.value("email", "").toString();
        nuser.expire = user.value("expire", "").toString();
        nuser.password = user.value("password", "").toString();
        nuser.token = user.value("token", "").toString();
        nuser.user_id = user.value("user_id", "").toString();
        nuser.user_name = user.value("user_name", "").toString();

        _douban->setUser(nuser);
    }
}

void MainWindow::saveBackupData() {
    QSettings settings("QDoubanFM", "DoubanFM");
    settings.setValue("channel", _channel);
    settings.setValue("volume", audioOutput->volume());
    QVariantMap user;
    DoubanUser curUser = _douban->getUser();
    user.insert("email", curUser.email);
    user.insert("expire", curUser.expire);
    user.insert("password", curUser.password);
    user.insert("token", curUser.token);
    user.insert("user_id", curUser.user_id);
    user.insert("user_name", curUser.user_name);
    settings.setValue("user", user);
    settings.sync();
}

MainWindow::~MainWindow() {
    saveBackupData();
    delete ui;
    delete mediaObject;
    delete audioOutput;
    delete _networkmgr;
    delete _douban;
}

void MainWindow::playTick(qint64 time) {
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
    ui->currentTick->setText(displayTime.toString("mm:ss"));
}


void MainWindow::getImage(const QString &url) {
    QString mod_url = url;
    mod_url.replace("mpic", "lpic");
    qDebug() << Q_FUNC_INFO << mod_url;
    _networkmgr->get(QNetworkRequest(QUrl(mod_url)));
}

void MainWindow::onReceivedImage(QNetworkReply *reply) {
    if (QNetworkReply::NoError != reply->error()) {
        qDebug() << Q_FUNC_INFO << "pixmap receiving error" << reply->error();
        reply->deleteLater();
        return;
    }
    const QByteArray data(reply->readAll());
    if (!data.size())
        qDebug() << Q_FUNC_INFO << "received pixmap looks like nothing";
    recvAlbumImage(data);
    reply->deleteLater();
}


void MainWindow::sourceChanged(const Phonon::MediaSource& source) {
    int index = mediaSources.indexOf(source);
    qDebug() << Q_FUNC_INFO << "Switch to" << songs[index].title;


    ui->artistName->setText(songs[index].artist);
    ui->albumName->setText(QString("< ") + songs[index].albumtitle + " >  " + songs[index].public_time);
    ui->songName->setText("<font color='green'>" + songs[index].title + "</font>");

    if (songs[index].like)
        ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton)));
    else
        ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogApplyButton)));

    getImage(songs[index].picture);
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
        ui->pauseButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_MediaPause)));
        unfreeze();
        pause_state = false;

        if (mediaSources.indexOf(mediaObject->currentSource()) == mediaSources.size() - 1) {
            _douban->getPlayingList(_channel, songs[songs.size() - 1].sid);
        }
        break;
    case Phonon::StoppedState:
        qDebug() << Q_FUNC_INFO << "Phonon::StoppedState";
        if (mediaSources.indexOf(mediaObject->currentSource()) == mediaSources.size() - 1) {
            _douban->getNewPlayList(_channel);
            freeze();
        }
        break;
    case Phonon::PausedState:
        qDebug() << Q_FUNC_INFO << "Phonon::PausedState";
        ui->pauseButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_MediaPlay)));
        pause_state = true;
        break;
    case Phonon::LoadingState:
        qDebug() << Q_FUNC_INFO << "Phonon::LoadingState";
        freeze();
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
    if (mediaObject->state() == Phonon::StoppedState) {
        _douban->getNewPlayList(_channel);
        return;
    }
    else if (mediaObject->state() == Phonon::PausedState) {
        mediaObject->play();
    }

    int index = mediaSources.indexOf(mediaObject->currentSource());
    _douban->skipSong(songs[index].sid, _channel);
    mediaObject->seek(mediaObject->totalTime());
}

void MainWindow::recvNewList(const QList<DoubanFMSong> &song) {
    this->songs = song;
    mediaSources.clear();
    foreach(DoubanFMSong s, song) {
        mediaSources.append(s.url);
    }
    mediaObject->clear();
    mediaObject->setQueue(mediaSources);
    mediaObject->play();
    unfreeze();
}

void MainWindow::recvPlayingList(const QList<DoubanFMSong> &song) {
    this->songs = song;
    mediaSources.clear();
    foreach(DoubanFMSong s, song) {
        mediaSources.append(s.url);
    }
    mediaObject->clearQueue();
    mediaObject->setQueue(mediaSources);
}

void MainWindow::recvAlbumImage(const QByteArray &data) {
    QPixmap pixmap;
    pixmap.loadFromData(data);
    int w = ui->albumImage->width();
    int h = ui->albumImage->height();
    ui->albumImage->setPixmap(pixmap.scaled(w, h, Qt::IgnoreAspectRatio));
}

void MainWindow::on_trashButton_clicked() {
    int index = mediaSources.indexOf(mediaObject->currentSource());
    qDebug() << Q_FUNC_INFO << "Bye Song.id =" << songs[index].sid;
    _douban->byeSong(songs[index].sid, _channel);
    mediaObject->seek(mediaObject->totalTime());
}

void MainWindow::on_heartButton_clicked() {
    int index = mediaSources.indexOf(mediaObject->currentSource());
    qDebug() << Q_FUNC_INFO << "Like Song.id =" << songs[index].sid;

    ui->heartButton->setEnabled(false);
    if (songs[index].like) {
        _douban->rateSong(songs[index].sid, _channel, false);
    }
    else {
        _douban->rateSong(songs[index].sid, _channel, true);
    }
}

static bool isInited = false;
void MainWindow::onReceivedChannels(const QList<DoubanChannel> &channels) {
    int index = 0;
    for (int i = 0; i < channels.size(); ++ i) {
        ui->channelComboBox->addItem(channels[i].name, QVariant(channels[i].channel_id));
        if (channels[i].channel_id == _channel) index = i;
    }
    ui->channelComboBox->setCurrentIndex(index);
    isInited = true;
}

void MainWindow::on_channelComboBox_currentIndexChanged(int index) {
    if (!isInited) return;

    _channel = ui->channelComboBox->itemData(index).toInt();

    mediaObject->stop();
    mediaObject->clear();
    _douban->getNewPlayList(_channel);
}

void MainWindow::recvRateSong(const bool succeed) {
    int index = mediaSources.indexOf(mediaObject->currentSource());
    ui->heartButton->setEnabled(true);
    if (succeed) {
        if (songs[index].like) {
            songs[index].like = false;
            ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogApplyButton)));
        }
        else {
            songs[index].like = true;
            ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton)));
        }
    }
}

void MainWindow::freeze() {
    ui->heartButton->setEnabled(false);
    ui->trashButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->channelComboBox->setEnabled(false);
}

void MainWindow::unfreeze() {
    ui->heartButton->setEnabled(true);
    ui->trashButton->setEnabled(true);
    ui->pauseButton->setEnabled(true);
    ui->channelComboBox->setEnabled(true);
}

void MainWindow::recvUserLogin(DoubanUser *user) {
    _douban->getNewPlayList(_channel);
    ui->userNameButton->setText(user->user_name);
}

void MainWindow::recvUserLogoff() {
    ui->userNameButton->setText("未登录");
}

void MainWindow::on_userNameButton_clicked() {
    _douban->userLogin();
}
