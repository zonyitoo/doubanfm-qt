#include "controlpanel.h"
#include "ui_controlpanel.h"
#include <QTime>
#include <QSettings>
#include <QMessageBox>
#include <QPropertyAnimation>

static QTime bktime;

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel)
{
    ui->setupUi(this);

    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(this);
    _douban = Douban::getInstance();

    ui->seekSlider->setMediaObject(mediaObject);
    Phonon::createPath(mediaObject, audioOutput);

    connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(playTick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(sourceChanged(Phonon::MediaSource)));

    connect(_douban, SIGNAL(receivedNewList(QList<DoubanFMSong>)),
            this, SLOT(recvNewList(QList<DoubanFMSong>)));
    connect(_douban, SIGNAL(receivedPlayingList(QList<DoubanFMSong>)),
            this, SLOT(recvPlayingList(QList<DoubanFMSong>)));

    connect(_douban, SIGNAL(receivedRateSong(bool)), this, SLOT(recvRateSong(bool)));

    connect(ui->userLoginWidget, SIGNAL(loginSucceed(DoubanUser)), this, SLOT(recvUserLoginSucceed(DoubanUser)));

    _channel = 2;
    this->loadBackupData();
    _douban->getNewPlayList(_channel);

    bktime.start();
}

ControlPanel::~ControlPanel()
{
    saveBackupData();
    delete ui;
    delete mediaObject;
    delete audioOutput;
    delete _douban;
}

int ControlPanel::getVisibleHeight() {
    return ui->trackingWidget->height();
}

qint32 ControlPanel::getCurrentChannel() {
    return _channel;
}

void ControlPanel::play() {
    emit mediaObject->play();
}

void ControlPanel::pause() {
    emit mediaObject->pause();
}

void ControlPanel::loadBackupData() {
    QSettings settings("QDoubanFM", "DoubanFM");

    _channel = settings.value("channel", 0).toInt();
    audioOutput->setVolume(settings.value("volume", 0.5).toDouble());
    ui->volumeWidget->setVolume(audioOutput->volume());
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

void ControlPanel::saveBackupData() {
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

void ControlPanel::playTick(qint64 time) {
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
    ui->currentTick->setText(QString("<font color='white'>") +
                             displayTime.toString("m:ss") + QString("</font>"));
}

void ControlPanel::sourceChanged(const Phonon::MediaSource& source) {
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
        //ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton)));
        ui->heartButton->setIcon(QIcon(":/icons/heart.png"));
    else
        //ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogApplyButton)));
        ui->heartButton->setIcon(QIcon(":/icons/heart_empty.png"));

    emit gotAlbumImage(songs[index].picture);
}

static bool pause_state = false;
void ControlPanel::stateChanged(Phonon::State newState, Phonon::State oldState) {
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

void ControlPanel::on_nextButton_clicked() {
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

void ControlPanel::recvNewList(const QList<DoubanFMSong> &song) {
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

void ControlPanel::recvPlayingList(const QList<DoubanFMSong> &song) {
    this->songs = song;
    mediaSources.clear();
    foreach(DoubanFMSong s, song) {
        mediaSources.append(s.url);
    }
    mediaObject->clearQueue();
    mediaObject->setQueue(mediaSources);
}

void ControlPanel::on_trashButton_clicked() {
    int index = mediaSources.indexOf(mediaObject->currentSource());
    qDebug() << Q_FUNC_INFO << "Bye Song.id =" << songs[index].sid;
    _douban->byeSong(songs[index].sid, _channel);
    mediaObject->seek(mediaObject->totalTime());
}

void ControlPanel::on_heartButton_clicked() {
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

void ControlPanel::recvRateSong(const bool succeed) {
    int index = mediaSources.indexOf(mediaObject->currentSource());
    ui->heartButton->setEnabled(true);
    if (succeed) {
        if (songs[index].like) {
            songs[index].like = false;
            //ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogApplyButton)));
            ui->heartButton->setIcon(QIcon(":/icons/heart_empty.png"));
        }
        else {
            songs[index].like = true;
            //ui->heartButton->setIcon(QIcon(this->style()->standardIcon(QStyle::SP_DialogCloseButton)));
            ui->heartButton->setIcon(QIcon(":/icons/heart.png"));
        }
    }
}

void ControlPanel::freeze() {
    ui->heartButton->setEnabled(false);
    ui->trashButton->setEnabled(false);
}

void ControlPanel::unfreeze() {
    ui->heartButton->setEnabled(true);
    ui->trashButton->setEnabled(true);
}

static bool isUserNamePanelShowing = false;

void ControlPanel::enterEvent(QEvent *event) {
    if (isUserNamePanelShowing) return;

    QPropertyAnimation *anim = new QPropertyAnimation(this, "geometry");
    anim->setDuration(300);

    anim->setStartValue(this->geometry());
    QRect curGeo = this->geometry();

    curGeo = QRect(curGeo.x(),
                   0
                        + ((QWidget *) this->parent())->geometry().height()
                        - this->geometry().height()
                        + ui->userLoginWidget->height(),
                   curGeo.width(),
                   curGeo.height());

    anim->setEndValue(curGeo);
    anim->setEasingCurve(QEasingCurve::OutQuad);
    anim->start();
}

void ControlPanel::leaveEvent(QEvent *event) {
    if (isUserNamePanelShowing) return;

    QPropertyAnimation *anim = new QPropertyAnimation(this, "geometry");
    anim->setDuration(300);

    anim->setStartValue(this->geometry());
    QRect curGeo = this->geometry();
    curGeo = QRect(curGeo.x(),
                   0
                        + ((QWidget *) this->parent())->geometry().height()
                        - ui->trackingWidget->geometry().height(),
                   curGeo.width(),
                   curGeo.height());
    anim->setEndValue(curGeo);
    anim->setEasingCurve(QEasingCurve::OutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    if (ui->volumeWidget->isShowing()) ui->volumeWidget->animHide();
}

void ControlPanel::onChannelChanged(qint32 channel) {
    _channel = channel;
    _douban->getNewPlayList(_channel);
}

void ControlPanel::on_volumeSlider_sliderMoved(int position) {
    this->audioOutput->setVolume(position / 100.0);
}

void ControlPanel::mousePressEvent(QMouseEvent *event) {

}

void ControlPanel::on_userNameButton_clicked() {
    QPropertyAnimation *anim = new QPropertyAnimation(this, "geometry");
    anim->setDuration(300);

    anim->setStartValue(this->geometry());
    QRect curGeo = this->geometry();
    if (!isUserNamePanelShowing) {
        curGeo = QRect(curGeo.x(),
                       0
                            + ((QWidget *) this->parent())->geometry().height()
                            - this->geometry().height(),
                       curGeo.width(),
                       curGeo.height());
        isUserNamePanelShowing = true;
    }
    else {
        curGeo = QRect(curGeo.x(),
                       0
                            + ((QWidget *) this->parent())->geometry().height()
                            - this->geometry().height()
                            + ui->userLoginWidget->height(),
                       curGeo.width(),
                       curGeo.height());

        isUserNamePanelShowing = false;

        DoubanUser duser = _douban->getUser();
        ui->userLoginWidget->setInfo(duser.email, duser.password);
    }
    anim->setEndValue(curGeo);
    anim->setEasingCurve(QEasingCurve::OutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void ControlPanel::on_volumeButton_clicked() {
    if (ui->volumeWidget->isShowing()) ui->volumeWidget->animHide();
    else ui->volumeWidget->animShow();
}

void ControlPanel::recvUserLoginSucceed(DoubanUser user) {
    _douban->setUser(user);
    _douban->getNewPlayList(_channel);

    QPropertyAnimation *anim = new QPropertyAnimation(this, "geometry");
    anim->setDuration(300);

    anim->setStartValue(this->geometry());
    QRect curGeo = this->geometry();

    curGeo = QRect(curGeo.x(),
                   0
                        + ((QWidget *) this->parent())->geometry().height()
                        - this->geometry().height()
                        + ui->userLoginWidget->height(),
                   curGeo.width(),
                   curGeo.height());
    isUserNamePanelShowing = false;

    anim->setEndValue(curGeo);
    anim->setEasingCurve(QEasingCurve::OutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
