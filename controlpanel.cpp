#include "controlpanel.h"
#include "ui_controlpanel.h"
#include <QLinearGradient>
#include <QPainter>
#include <QPolygon>
#include <QRegion>
#include <QDebug>
#include <QTime>
#include <QSettings>
#include <QNetworkReply>
#include "mainwidget.h"
#include <QDesktopServices>
#include "libs/douban_types.h"
#include "settingdialog.h"

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel),
    doubanfm(DoubanFM::getInstance()),
    player(DoubanPlayer::getInstance()),
    imgmgr(new QNetworkAccessManager(this)),
    lyricGetter(new LyricGetter(this)),
    settingDialog(new SettingDialog(this))
{
    ui->setupUi(this);
    loadConfig();

    // Shape of channelButton and lyricButton
    QPolygon polygonTop, polygonBottom;
    polygonTop.setPoints(4,   0, 0,   131, 0,   115, 16,    16, 16);
    polygonBottom.setPoints(4,    16, 0,    115, 0,    131, 16,    0, 16);
    QRegion regionTop(polygonTop);
    QRegion regionBottom(polygonBottom);
    ui->channelButton->setMask(regionTop);
    ui->lyricButton->setMask(regionBottom);

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
    connect(&player, &DoubanPlayer::currentSongChanged, [=] (const DoubanFMSong& song) {
        setArtistName(song.artist);
        setSongName(song.title);
        setAlbumName(song.albumtitle);

        QString mod_url = song.picture;
        mod_url.replace("mpic", "lpic");
        imgmgr->get(QNetworkRequest(QUrl(mod_url)));
        if (song.like) {
            ui->likeButton->setChecked(true);
        }
        else {
            ui->likeButton->setChecked(false);
        }
    });

    setArtistName("Loading");

    connect(&player, SIGNAL(positionChanged(qint64)), ui->volumeTime, SLOT(setTick(qint64)));
    connect(&player, &DoubanPlayer::positionChanged, [this] (qint64 tick) {
        ui->seeker->setValue((qreal) tick / player.duration() * ui->seeker->maximum());
    });
    //connect(player, &QMediaPlayer::volumeChanged, [this] (int vol) {
    //    qDebug() << vol;
    //});

    connect(&player, &DoubanPlayer::receivedRateSong, [this] (bool succeed) {
        if (!succeed) return;

        if (player.currentSong().like) {
            ui->likeButton->setChecked(true);
        }
        else {
            ui->likeButton->setChecked(false);
        }
    });

    connect(ui->volumeTime, &VolumeTimePanel::volumeChanged, [this] (int value) {
        this->player.setVolume(value);
    });

    /*if (player->channel() == -3) {
        if (!doubanfm->getUser())
            player->setChannel(1);
    }*/

    connect(lyricGetter, &LyricGetter::gotLyricError, [this] (const QString& ) {
        /*if (ui->lyricWidget->isVisible())
            emit ui->lyricWidgetTriggerRight->enter();*/
    });

    //ui->lyricWidget->setVisible(false);
    connect(ui->albumImg, &AlbumWidget::clicked, [this] () {
        QDesktopServices::openUrl(QUrl("http://www.douban.com" + player.currentSong().album));
    });

    connect(&player, &DoubanPlayer::canControlChanged, [=] (bool can) {
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
    delete lyricGetter;
    delete settingDialog;
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
    player.setVolume(settings.value("volume", 100).toInt());
    qint32 _channel = settings.value("channel", 1).toInt();
    player.setKbps(settings.value("kbps", 64).toInt());
    settings.endGroup();

    if (_channel == -3 && doubanfm.hasLogin()) {
        player.setChannel(_channel);
    }
}

void ControlPanel::saveConfig() {
    QSettings settings("QDoubanFM", "QDoubanFM");
    settings.beginGroup("General");
    settings.setValue("channel", player.channel());
    settings.setValue("volume", player.volume());
    settings.setValue("kbps", player.kbps());
    settings.endGroup();
}

void ControlPanel::on_nextButton_clicked()
{
    ui->seeker->setValue(0);
    player.next();
}

void ControlPanel::on_pauseButton_clicked()
{
    player.pause();
}

void ControlPanel::on_likeButton_clicked()
{
    bool is_liked = player.currentSong().like;
    if (is_liked)
        player.unrateCurrentSong();
    else
        player.rateCurrentSong();
}

void ControlPanel::on_trashButton_clicked()
{
    player.trashCurrentSong();
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
    player.play();
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
    player.pause();
}

void ControlPanel::on_settingButton_clicked()
{
    settingDialog->show();
}

void ControlPanel::on_channelButton_clicked(bool checked)
{
    if (checked)
        emit openChannelPanel();
    else
        emit closeChannelPanel();
}

void ControlPanel::on_lyricButton_clicked(bool checked)
{
    if (checked)
        emit openLyricPanel();
    else
        emit closeLyricPanel();
}
