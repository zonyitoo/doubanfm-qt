#include "mainui.h"
#include "ui_mainui.h"
#include <qjson/parser.h>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QScrollBar>
#include "channelbutton.h"

static QTime bktime;

MainUI::MainUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainUI)
{
    ui->setupUi(this);
    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(this);
    _douban = Douban::getInstance();
    _networkmgr = new QNetworkAccessManager(this);

    ui->seekSlider->setMediaObject(mediaObject);
    ui->volumeSlider->setAudioOutput(audioOutput);
    Phonon::createPath(mediaObject, audioOutput);

    ui->trashButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_TrashIcon)));
    ui->pauseButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_MediaPause)));
    ui->nextButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_MediaSkipForward)));
    ui->userNameButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DirHomeIcon)));

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
    connect(_douban, SIGNAL(loginFailed(QString)), this, SLOT(recvUserLoginFailed(QString)));

    _channel = 2;
    this->loadBackupData();
    _douban->getChannels();
    _douban->getNewPlayList(_channel);

    ui->userNameLineEdit->hide();
    ui->passwordLineEdit->hide();
    ui->loginButton->hide();
    ui->loginErrMsg->hide();

    ui->scrollArea->hide();
    ui->channelButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_ArrowDown)));

    emailRegExp.setPatternSyntax(QRegExp::RegExp2);
    emailRegExp.setCaseSensitivity(Qt::CaseSensitive);
    emailRegExp.setPattern("^[a-zA-Z_]+([\\.-]?[a-zA-Z_]+)*@[a-zA-Z_]+([\\.-]?[a-zA-Z_]+)*(\\.[a-zA-Z_]{2,3})+");

    /*
    if (_douban->hasLogin()) {
        ui->userNameButton->setText(_douban->getUser().user_name);
    }
    else {
        ui->userNameButton->setText(QString("未登录"));
    }
    */

    bktime.start();
}

void MainUI::loadBackupData() {
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

void MainUI::saveBackupData() {
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

MainUI::~MainUI() {
    saveBackupData();
    delete ui;
    delete mediaObject;
    delete audioOutput;
    delete _networkmgr;
    delete _douban;
}

void MainUI::playTick(qint64 time) {
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
    ui->currentTick->setText(QString("<font color='white'>") +
                             displayTime.toString("mm:ss") + QString("</font>"));
}


void MainUI::getImage(const QString &url) {
    QString mod_url = url;
    mod_url.replace("mpic", "lpic");
    qDebug() << Q_FUNC_INFO << mod_url;
    _networkmgr->get(QNetworkRequest(QUrl(mod_url)));
}

void MainUI::onReceivedImage(QNetworkReply *reply) {
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


void MainUI::sourceChanged(const Phonon::MediaSource& source) {
    int index = mediaSources.indexOf(source);
    qDebug() << Q_FUNC_INFO << "Switch to" << songs[index].title;


    ui->artistName->setText(QString("<font color=grey>")
                            + songs[index].artist
                            + QString(" - &lt; ")
                            + songs[index].albumtitle
                            + " &gt;  "
                            + songs[index].public_time
                            + QString("</font>"));

    ui->songName->setText("<font color='#57463e'>" + songs[index].title + "</font>");

    if (songs[index].like)
        ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton)));
    else
        ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogApplyButton)));

    getImage(songs[index].picture);
}

static bool pause_state = false;
void MainUI::stateChanged(Phonon::State newState, Phonon::State oldState) {
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

        if (oldState == Phonon::PausedState) {
            if (bktime.restart() > 1800000) {
                _douban->getPlayingList(_channel,
                                        songs[mediaSources.indexOf(mediaObject->currentSource())].sid);
            }
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

        if (oldState == Phonon::PlayingState) {
            bktime.restart();
        }

        break;
    case Phonon::LoadingState:
        qDebug() << Q_FUNC_INFO << "Phonon::LoadingState";
        freeze();
        break;
    }
}

void MainUI::on_pauseButton_clicked() {
    if (pause_state) {
        emit mediaObject->play();
    }
    else {
        emit mediaObject->pause();
    }
}

void MainUI::on_nextButton_clicked() {
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

void MainUI::recvNewList(const QList<DoubanFMSong> &song) {
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

void MainUI::recvPlayingList(const QList<DoubanFMSong> &song) {
    this->songs = song;
    mediaSources.clear();
    foreach(DoubanFMSong s, song) {
        mediaSources.append(s.url);
    }
    mediaObject->clearQueue();
    mediaObject->setQueue(mediaSources);
}

void MainUI::recvAlbumImage(const QByteArray &data) {
    QPixmap pixmap;
    pixmap.loadFromData(data);
    int w = ui->albumImage->width();
    int h = ui->albumImage->height();
    ui->albumImage->setPixmap(pixmap.scaled(w, h, Qt::IgnoreAspectRatio));

}

void MainUI::on_trashButton_clicked() {
    int index = mediaSources.indexOf(mediaObject->currentSource());
    qDebug() << Q_FUNC_INFO << "Bye Song.id =" << songs[index].sid;
    _douban->byeSong(songs[index].sid, _channel);
    mediaObject->seek(mediaObject->totalTime());
}

void MainUI::on_heartButton_clicked() {
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
void MainUI::onReceivedChannels(const QList<DoubanChannel> &channels) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    for (int i = 0; i < channels.size(); ++ i) {
        ChannelButton *bt = new ChannelButton(ui->scrollArea);
        bt->setText(channels[i].name);
        bt->setFlat(true);
        layout->addWidget(bt);
        connect(bt, SIGNAL(clicked(const ChannelButton*)),
                this, SLOT(on_chooseChannel(const ChannelButton*)));

        if (channels[i].channel_id == _channel) {
            ui->channelButton->setText(channels[i].name);
        }
    }
    ui->scrollArea->widget()->setLayout(layout);
    isInited = true;

    this->channels = channels;
}

void MainUI::on_chooseChannel(const ChannelButton *button) {
    foreach(DoubanChannel c, channels) {
        if (c.name == button->text()) {
            _channel = c.channel_id;
            _douban->getNewPlayList(_channel);
            ui->channelButton->setText(c.name);
            break;
        }
    }
}

void MainUI::recvRateSong(const bool succeed) {
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

void MainUI::freeze() {
    ui->heartButton->setEnabled(false);
    ui->trashButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->channelButton->setEnabled(false);
}

void MainUI::unfreeze() {
    ui->heartButton->setEnabled(true);
    ui->trashButton->setEnabled(true);
    ui->pauseButton->setEnabled(true);
    ui->channelButton->setEnabled(true);
}

void MainUI::recvUserLogin(DoubanUser *user) {
    _douban->getNewPlayList(_channel);
    //ui->userNameButton->setText(user->user_name);
    user->password = ui->passwordLineEdit->text().trimmed();

    ui->userNameLineEdit->hide();
    ui->passwordLineEdit->hide();
    ui->loginButton->hide();
    ui->loginErrMsg->hide();

    ui->userNameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginButton->setEnabled(true);
}

void MainUI::recvUserLoginFailed(const QString &errmsg) {
    ui->loginErrMsg->setText(QString("<font color=red>") + errmsg + QString("</font>"));

    ui->userNameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginButton->setEnabled(true);
}

void MainUI::recvUserLogoff() {
    /*
    ui->userNameButton->setText(QString("未登录"));
    */
}

void MainUI::on_userNameButton_clicked() {
    if (ui->userNameLineEdit->isHidden()) {
        if (_douban->hasLogin()) {
            DoubanUser u = _douban->getUser();
            ui->userNameLineEdit->setText(u.email);
            ui->passwordLineEdit->setText(u.password);

            ui->loginErrMsg->setText(QString("<font color=green>用户 ")
                                     + u.email + QString(" 已登录</font>"));
        }
        else {
            ui->loginErrMsg->setText("<font color=green>输入用户名和密码</font>");
        }

        ui->userNameLineEdit->show();
        ui->passwordLineEdit->show();
        ui->loginButton->show();
        ui->loginErrMsg->show();
    }
    else {
        ui->userNameLineEdit->hide();
        ui->passwordLineEdit->hide();
        ui->loginButton->hide();
        ui->loginErrMsg->hide();
    }
}

void MainUI::on_loginButton_clicked() {
    QString em = ui->userNameLineEdit->text().trimmed();
    QString pw = ui->passwordLineEdit->text().trimmed();

    if (emailRegExp.exactMatch(em) && !pw.isEmpty()) {
        _douban->doLogin(em, pw);

        ui->userNameLineEdit->setEnabled(false);
        ui->passwordLineEdit->setEnabled(false);
        ui->loginButton->setEnabled(false);
        ui->loginErrMsg->setText(QString("<font color=green>登录中</font>"));
    }
    else {
        ui->loginErrMsg->setText(QString("<font color=red>请输入正确格式的用户名和密码</font>"));
    }
}

void MainUI::on_channelButton_clicked() {
    if (ui->scrollArea->isHidden()) {
        ui->scrollArea->show();
        ui->channelButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_ArrowUp)));
    }
    else {
        ui->scrollArea->hide();
        ui->channelButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_ArrowDown)));
    }
}
