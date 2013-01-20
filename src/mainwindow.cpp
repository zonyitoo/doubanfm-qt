#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qjson/parser.h>
#include <QMessageBox>
#include <QPixmap>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);
    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(this);
    seekSlider = new Phonon::SeekSlider(this);
    volumeSilder = new Phonon::VolumeSlider(this);
    _douban = Douban::getInstance();
    _networkmgr = new QNetworkAccessManager(this);

    ui->controlLayout->addWidget(seekSlider);
    QHBoxLayout *volumeControlLayout = new QHBoxLayout(this);
    volumeControlLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    volumeControlLayout->addWidget(volumeSilder);
    ui->controlLayout->addItem(volumeControlLayout);

    seekSlider->setMediaObject(mediaObject);
    seekSlider->setEnabled(false);
    volumeSilder->setAudioOutput(audioOutput);
    Phonon::createPath(mediaObject, audioOutput);

    ui->trashButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_TrashIcon)));
    ui->pauseButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_MediaPause)));
    ui->nextButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_MediaSkipForward)));

    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(playTick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(sourceChanged(Phonon::MediaSource)));

    connect(_douban, SIGNAL(receivedNewList(QList<DoubanFMSong>)),
            this, SLOT(recvNewList(QList<DoubanFMSong>)));
    connect(_douban, SIGNAL(receivedPlayingList(QList<DoubanFMSong>)),
            this, SLOT(recvPlayingList(QList<DoubanFMSong>)));
    connect(_douban, SIGNAL(receivedChannels(QList<DoubanChannel>)),
            this, SLOT(onReceivedChannels(QList<DoubanChannel>)));
    connect(_douban, SIGNAL(receivedRateSong(bool)), this, SLOT(recvRateSong(bool)));

    _channel = 1;
    this->loadBackupData("config.xml");
    _douban->getChannels();
    _douban->getNewPlayList(_channel);
}

void MainWindow::loadBackupData(const QString& filename) {
    QDomDocument doc;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return;
    if (doc.setContent(&file)) {
        QDomElement config = doc.documentElement();
        QDomNodeList list = config.childNodes();
        for (int i = 0; i < list.size(); ++ i) {
            if (list.at(i).nodeName() == "channel") {
                bool ok;
                _channel = list.at(i).toElement().text().toInt(&ok, 10);
                if (!ok) _channel = 0;
                qDebug() << Q_FUNC_INFO << "Last channel =" << _channel;
            }
            else if (list.at(i).nodeName() == "volume") {
                bool ok;
                audioOutput->setVolume(list.at(i).toElement().text().toDouble(&ok));
                if (!ok) audioOutput->setVolume(0.5);
                qDebug() << Q_FUNC_INFO << "Last volume =" << audioOutput->volume();
            }
            else if (list.at(i).nodeName() == "user") {
                QDomElement user = list.at(i).toElement();
                QString user_id = user.attribute("user_id");

                QString expire, token;

                QDomNodeList list = user.childNodes();
                for (int i = 0; i < list.size(); ++ i) {
                    QDomNode node = list.at(i);
                    if (node.nodeName() == "expire") {
                        expire = node.toElement().text();
                    }
                    else if (node.nodeName() == "token") {
                        token = node.toElement().text();
                    }
                }

                if (user_id.isEmpty() || expire.isEmpty() || token.isEmpty()) continue;

                DoubanUser ruser;
                ruser.user_id = user_id;
                ruser.expire = expire;
                ruser.token = token;
                _douban->setUser(ruser);
            }
        }
    }
    file.close();
}

void MainWindow::saveBackupData(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");

    QDomDocument doc;

    QDomProcessingInstruction instruction =
            doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement doubanfm = doc.createElement("DoubanFM");
    QDomElement channel = doc.createElement("channel");
    QDomText channel_val = doc.createTextNode(QString::number(_channel));
    channel.appendChild(channel_val);
    QDomElement volume = doc.createElement("volume");
    QDomText volume_val = doc.createTextNode(QString::number(audioOutput->volume()));
    volume.appendChild(volume_val);
    doubanfm.appendChild(volume);
    doubanfm.appendChild(channel);

    QDomElement user = doc.createElement("user");
    user.setAttribute("user_id", _douban->getUser().user_id);
    QDomElement expire = doc.createElement("expire");
    QDomText expire_t = doc.createTextNode(_douban->getUser().expire);
    expire.appendChild(expire_t);
    QDomElement token = doc.createElement("token");
    QDomText token_t = doc.createTextNode(_douban->getUser().token);
    token.appendChild(token_t);
    user.appendChild(expire);
    user.appendChild(token);
    doubanfm.appendChild(user);

    doc.appendChild(doubanfm);
    doc.save(out, 4, QDomNode::EncodingFromTextStream);

    file.close();
}

MainWindow::~MainWindow() {
    saveBackupData("config.xml");
    delete ui;
    delete mediaObject;
    delete audioOutput;
    delete seekSlider;
    delete _networkmgr;
    delete _douban;
}

void MainWindow::playTick(qint64 time) {
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
    ui->currentTick->setText(displayTime.toString("mm:ss"));
}


void MainWindow::getImage(const QString &url) {
    qDebug() << Q_FUNC_INFO << url;
    disconnect(_networkmgr, 0, 0, 0);
    connect(_networkmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReceivedImage(QNetworkReply*)));
    _networkmgr->get(QNetworkRequest(QUrl(url)));
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
    int index = mediaSources.indexOf(mediaObject->currentSource());
    _douban->skipSong(songs[index].sid, _channel);
    mediaObject->seek(mediaObject->totalTime());
}

void MainWindow::recvNewList(const QList<DoubanFMSong> &song) {
    qDebug() << Q_FUNC_INFO;
    this->songs = song;
    mediaSources.clear();
    foreach(DoubanFMSong s, song) {
        mediaSources.append(s.url);
        qDebug() << Q_FUNC_INFO << s.title;
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
        qDebug() << Q_FUNC_INFO << s.title;
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
        qDebug() << Q_FUNC_INFO << channels[i].name << channels[i].channel_id;
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
