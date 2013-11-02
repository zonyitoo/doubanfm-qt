#include "controlpanel.h"
#include "ui_controlpanel.h"
#include <QLinearGradient>
#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QSettings>
#include <QNetworkReply>
#include "mainwidget.h"
#include <QDesktopServices>
#include "libs/douban_types.h"

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel),
    doubanfm(DoubanFM::getInstance()),
    player(DoubanPlayer::getInstance()),
    imgmgr(new QNetworkAccessManager(this)),
    lyric_getter(new LyricGetter(this))
{
    ui->setupUi(this);
    loadConfig();

    connect(imgmgr, &QNetworkAccessManager::finished, [this] (QNetworkReply *reply) {
        if (QNetworkReply::NoError != reply->error()) {
            qDebug() << "Err: Album image receive error";
            reply->deleteLater();
            return;
        }
        const QByteArray data(reply->readAll());
        if (!data.size())
            qDebug() << Q_FUNC_INFO << "received pixmap looks like nothing";
        QImage image = QImage::fromData(data);
        /*
        if (player.playlist()->currentIndex() >= 0) {
            int index = player.playlist()->currentIndex();
            if (notify) {
                notify->close();
                delete notify;
                notify = nullptr;
            }
            notify = new Notification(songs[index].artist, songs[index].title);
            if (data.size() > 0) {
                iiibiiay notify_icon_data = iiibiiay::fromImage(image);
                notify->setHint("icon_data",
                                QVariant(qDBusRegisterMetaType<iiibiiay>(), &notify_icon_data));
            }
            notify->setAutoDelete(false);
            notify->show();
        }
        */
        ui->albumImg->setAlbumImage(image);
        reply->deleteLater();
    });
    
    //player.setPlaylist(new QMediaPlaylist(&player));
    connect(player, &DoubanPlayer::currentSongChanged, [=] (const DoubanFMSong& song) {
        setArtistName(song.artist);
        setSongName(song.title);
        setAlbumName(song.albumtitle);

        ui->lyricWidget->clear();
        this->lyric_getter->getLyric(song.title, song.artist);
        QString mod_url = song.picture;
        mod_url.replace("mpic", "lpic");
        imgmgr->get(QNetworkRequest(QUrl(mod_url)));
        if (song.like) {
            ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}"
                                          "QToolButton:hover{border-image: url(:/img/unlike.png);}"
                                          "QToolButton:clicked{border-image: url(:/img/like_disabled.png);}"
                                          "QToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
        }
        else {
            ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}"
                                          "QToolButton:hover{border-image: url(:/img/like.png);}"
                                          "QToolButton:clicked{border-image: url(:/img/like_disabled.png);}"
                                          "QToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
        }
    });

    setArtistName("Loading");

    connect(doubanfm, &DoubanFM::loginSucceed, [this] (std::shared_ptr<DoubanUser> user) {
        ui->userLogin->setText(user->user_name);
    });

    connect(player, SIGNAL(positionChanged(qint64)), ui->volumeTime, SLOT(setTick(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), ui->lyricWidget, SLOT(setTick(qint64)));
    connect(player, &DoubanPlayer::positionChanged, [this] (qint64 tick) {
        ui->seeker->setValue((qreal) tick / player->duration() * ui->seeker->maximum());
    });
    //connect(player, &QMediaPlayer::volumeChanged, [this] (int vol) {
    //    qDebug() << vol;
    //});

    connect(player, &DoubanPlayer::receivedRateSong, [this] (bool succeed) {
        if (!succeed) return;

        if (player->currentSong().like) {
            ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}"
                                          "QToolButton:hover{border-image: url(:/img/unlike.png);}"
                                          "QToolButton:clicked{border-image: url(:/img/like_disabled.png);}"
                                          "QToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
        }
        else {
            ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}"
                                          "QToolButton:hover{border-image: url(:/img/like.png);}"
                                          "QToolButton:clicked{border-image: url(:/img/like_disabled.png);}"
                                          "QToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
        }
    });

    connect(static_cast<mainwidget *>(this->parentWidget())->channelWidget(), &ChannelWidget::channelChanged,
            [this] (qint32 channel) {
        this->player->setChannel(channel);
    });
    connect(ui->volumeTime, &VolumeTimePanel::volumeChanged, [this] (int value) {
        this->player->setVolume(value);
    });

    /*if (player->channel() == -3) {
        if (!doubanfm->getUser())
            player->setChannel(1);
    }*/

    if (doubanfm->hasLogin())
        ui->userLogin->setText(doubanfm->getUser()->user_name);


    connect(lyric_getter, &LyricGetter::gotLyric, [this] (const QLyricList& lyric) {
        ui->lyricWidget->setLyric(lyric);
    });
    connect(lyric_getter, &LyricGetter::gotLyricError, [this] (const QString& errmsg) {
        /*if (ui->lyricWidget->isVisible())
            emit ui->lyricWidgetTriggerRight->enter();*/
    });

    //ui->lyricWidget->setVisible(false);
    connect(ui->albumImg, &AlbumWidget::clicked, [this] () {
        /*if (!ui->lyricWidget->isVisible())
            emit ui->lyricWidgetTriggerLeft->enter();
        else
            emit ui->lyricWidgetTriggerRight->enter();*/
        QDesktopServices::openUrl(QUrl("http://www.douban.com" + player->currentSong().album));
    });

    connect(ui->channelWidgetTrigger, &ChannelWidgetTrigger::enter,
            [this] () {
        if (!static_cast<mainwidget *>(this->parentWidget())->isChannelWidgetShowing())
            static_cast<mainwidget *>(this->parentWidget())->animShowChannelWidget();

    });
    connect(dynamic_cast<mainwidget *>(this->parentWidget())->channelWidget(),
            &ChannelWidget::channelChanged, [=] (qint32 chanid) {
        this->player->setChannel(chanid);
    });
    connect(player, &DoubanPlayer::canControlChanged, [=] (bool can) {
        ui->nextButton->setEnabled(can);
        ui->trashButton->setEnabled(can);
        ui->likeButton->setEnabled(can);
    });
    /*
    connect(ui->lyricWidgetTriggerLeft, &LyricWidgetTriggerLeft::enter, [this] () {
        QPropertyAnimation *anim = new QPropertyAnimation(ui->albumImg, "geometry");
        anim->setDuration(400);
        anim->setStartValue(ui->albumImg->geometry());
        QRect endval(this->geometry().width() - ui->albumImg->geometry().width(),
                   ui->albumImg->geometry().y(),
                   ui->albumImg->geometry().width(),
                   ui->albumImg->geometry().height());
        anim->setEndValue(endval);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        ui->album->setVisible(false);
        ui->artist->setVisible(false);
        ui->volumeTime->setVisible(false);
        ui->userLogin->setVisible(false);
        ui->trashButton->setVisible(false);
        ui->songName->setVisible(false);
        ui->seeker->setVisible(false);
        ui->pauseButton->setVisible(false);
        ui->nextButton->setVisible(false);
        ui->likeButton->setVisible(false);

        connect(anim, &QPropertyAnimation::finished, [this] () {
            ui->lyricWidget->setVisible(true);
        });

        anim->start(QPropertyAnimation::DeleteWhenStopped);
    });
    connect(ui->lyricWidgetTriggerRight, &LyricWidgetTriggerRight::enter, [this] () {
        QPropertyAnimation *anim = new QPropertyAnimation(ui->albumImg, "geometry");
        anim->setDuration(400);
        anim->setStartValue(ui->albumImg->geometry());
        QRect endval(0,
                   ui->albumImg->geometry().y(),
                   ui->albumImg->geometry().width(),
                   ui->albumImg->geometry().height());
        anim->setEndValue(endval);
        anim->setEasingCurve(QEasingCurve::OutCubic);

        ui->lyricWidget->setVisible(false);

        connect(anim, &QPropertyAnimation::finished, [this] () {
            ui->album->setVisible(true);
            ui->artist->setVisible(true);
            ui->volumeTime->setVisible(true);
            ui->userLogin->setVisible(true);
            ui->trashButton->setVisible(true);
            ui->songName->setVisible(true);
            ui->seeker->setVisible(true);
            ui->pauseButton->setVisible(true);
            ui->nextButton->setVisible(true);
            ui->likeButton->setVisible(true);
        });

        anim->start(QPropertyAnimation::DeleteWhenStopped);
    });
    */
}

ControlPanel::~ControlPanel()
{
    delete ui;
    saveConfig();
    delete lyric_getter;
}

void ControlPanel::setSongName(const QString &name) {
    ui->songName->setText(QString("<font color='#04aaa1'>")
                          + name + QString("</font>"));
}

void ControlPanel::setArtistName(const QString &name) {
    ui->artist->setText(QString("<font color='grey'>")
                          + name + QString("</font>"));
}

void ControlPanel::loadConfig() {
    QSettings settings("QDoubanFM", "QDoubanFM");

    settings.beginGroup("General");
    player->setVolume(settings.value("volume", 100).toInt());
    qint32 _channel = settings.value("channel", 1).toInt();
    settings.endGroup();

    if (_channel == -3 && doubanfm->hasLogin()) {
        player->setChannel(_channel);
    }
}

void ControlPanel::saveConfig() {
    QSettings settings("QDoubanFM", "QDoubanFM");
    settings.beginGroup("General");
    settings.setValue("channel", player->channel());
    settings.setValue("volume", player->volume());
    settings.endGroup();
}

void ControlPanel::on_nextButton_clicked()
{
    if (static_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        static_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    ui->seeker->setValue(0);
    player->next();
}

void ControlPanel::on_pauseButton_clicked()
{
    if (static_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        static_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    player->pause();
    static_cast<mainwidget *>(this->parentWidget())->pauseMask()->setVisible(true);
}

void ControlPanel::on_likeButton_clicked()
{
    if (static_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        static_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    bool is_liked = player->currentSong().like;
    if (is_liked)
        player->unrateCurrentSong();
    else
        player->rateCurrentSong();
}

void ControlPanel::on_trashButton_clicked()
{
    if (static_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        static_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
        return;
    }

    player->trashCurrentSong();
}

void ControlPanel::on_userLogin_clicked()
{
    LoginPanel *loginPanel = static_cast<mainwidget *>(this->parentWidget())->loginPanel();
    if (!loginPanel->isShowing())
        loginPanel->animShow();
    else
        loginPanel->animHide();
}

void ControlPanel::setAlbumName(const QString &name) {
    ui->album->setText(QString("<font color=grey>&lt; ") + name + QString(" &gt;</font>"));
}

void ControlPanel::play() {
    /*QPropertyAnimation *fadein = new QPropertyAnimation(&player, "volume");
    fadein->setDuration(1000);
    fadein->setStartValue(player.volume());
    player.play();
    fadein->setEndValue(volume);
    fadein->start(QPropertyAnimation::DeleteWhenStopped);
    isPaused = false;*/
    player->play();
}

void ControlPanel::pause() {
    /*QPropertyAnimation *fadeout = new QPropertyAnimation(&player, "volume");
    fadeout->setDuration(1000);
    fadeout->setStartValue(player.volume());
    volume = player.volume();
    fadeout->setEndValue(0);
    connect(fadeout, &QPropertyAnimation::finished, [this] () {
        player.pause();
    });
    fadeout->start(QPropertyAnimation::DeleteWhenStopped);
    isPaused = true;*/
    player->pause();
}

void ControlPanel::enterEvent(QEvent *ev) {
    static_cast<mainwidget *>(this->parentWidget())->animHideChannelWidget(true);
}
