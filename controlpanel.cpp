#include "controlpanel.h"
#include "ui_controlpanel.h"
#include <QLinearGradient>
#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QSettings>
#include <QNetworkReply>
#include "mainwidget.h"

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel),
    channel(0), isPaused(false), volume(100)
{
    ui->setupUi(this);
    doubanfm = DoubanFM::getInstance();
    loadConfig();
    imgmgr = new QNetworkAccessManager(this);
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
        ui->albumImg->setAlbumImage(image);
        reply->deleteLater();
    });

    setArtistName("Loading");

    connect(doubanfm, &DoubanFM::loginSucceed, [this] (std::shared_ptr<DoubanUser> user) {
        doubanfm->getNewPlayList(channel);
        ui->userLogin->setText(user->user_name);
        qDebug() << "LoginSucceed. Refreshing Playlist";
    });
    connect(doubanfm, &DoubanFM::receivedNewList, [this] (const QList<DoubanFMSong>& songs) {
        this->songs = songs;
        qDebug() << "Received new playlist with" << songs.size() << "songs";
        QMediaPlaylist *playlist = new QMediaPlaylist;
        for (const DoubanFMSong& song : this->songs) {
            playlist->addMedia(QUrl(song.url));
        }
        player.setPlaylist(playlist);
        connect(playlist, &QMediaPlaylist::currentIndexChanged, [=] (int position) {
            if (position < 0) {
                if (songs.size() > 0) {
                    doubanfm->getPlayingList(channel, songs.back().sid);
                }
                else {
                    doubanfm->getNewPlayList(channel);
                }
                return;
            }
            qDebug() << "Current playing: " << songs[position].artist << ":" << songs[position].title;
            setArtistName(songs[position].artist);
            setSongName(songs[position].title);
            setAlbumName(songs[position].albumtitle);
            lyric_getter->getLyric(songs[position].title, songs[position].artist == "Various Artists" ? "" : songs[position].artist);
            QString mod_url = songs[position].picture;
            mod_url.replace("mpic", "lpic");
            ui->lyricWidget->clear();
            imgmgr->get(QNetworkRequest(QUrl(mod_url)));
            if (songs[position].like) {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}\nQToolButton:hover{border-image: url(:/img/unlike.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
            }
            else {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}\nQToolButton:hover{border-image: url(:/img/like.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
            }
        });
        if (player.state() != QMediaPlayer::PlayingState) {
            setArtistName(songs[0].artist);
            setSongName(songs[0].title);
            setAlbumName(songs[0].albumtitle);
            lyric_getter->getLyric(songs[0].title, songs[0].artist == "Various Artists" ? "" : songs[0].artist);
            QString mod_url = songs[0].picture;
            mod_url.replace("mpic", "lpic");
            imgmgr->get(QNetworkRequest(QUrl(mod_url)));
            player.play();
            ui->lyricWidget->clear();
            if (songs[0].like) {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}\nQToolButton:hover{border-image: url(:/img/unlike.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
            }
            else {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}\nQToolButton:hover{border-image: url(:/img/like.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
            }
            qDebug() << "Current playing: " << songs[0].artist << ":" << songs[0].title;
        }
    });

    connect(&player, SIGNAL(positionChanged(qint64)), ui->volumeTime, SLOT(setTick(qint64)));
    connect(&player, SIGNAL(positionChanged(qint64)), ui->lyricWidget, SLOT(setTick(qint64)));
    connect(&player, &QMediaPlayer::positionChanged, [this] (qint64 tick) {
        ui->seeker->setValue((qreal) tick / player.duration() * 100);
    });

    connect(doubanfm, &DoubanFM::receivedSkipSong, [this] (bool succeed) {
        ui->nextButton->setEnabled(true);
    });
    connect(doubanfm, &DoubanFM::receivedByeSong, [this] (bool succeed) {
        ui->trashButton->setEnabled(true);
        player.playlist()->setCurrentIndex(player.playlist()->nextIndex());
    });
    connect(doubanfm, &DoubanFM::receivedRateSong, [this] (bool succeed) {
        ui->likeButton->setEnabled(true);
        if (!succeed) return;
        int index = player.playlist()->currentIndex();
        songs[index].like = !songs[index].like;
        if (songs[index].like) {
            ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}\nQToolButton:hover{border-image: url(:/img/unlike.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
        }
        else {
            ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}\nQToolButton:hover{border-image: url(:/img/like.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
        }
    });

    connect(doubanfm, &DoubanFM::receivedPlayingList, [this] (const QList<DoubanFMSong>& songs) {
        this->songs = songs;
        qDebug() << "Received new playlist with" << songs.size() << "songs";
        QMediaPlaylist *playlist = new QMediaPlaylist;
        for (const DoubanFMSong& song : this->songs) {
            playlist->addMedia(QUrl(song.url));
        }
        player.setPlaylist(playlist);
        connect(playlist, &QMediaPlaylist::currentIndexChanged, [=] (int position) {
            if (position < 0) {
                if (songs.size() > 0) {
                    doubanfm->getPlayingList(channel, songs.back().sid);
                }
                else {
                    doubanfm->getNewPlayList(channel);
                }
                return;
            }
            setArtistName(songs[position].artist);
            setSongName(songs[position].title);
            setAlbumName(songs[position].albumtitle);
            lyric_getter->getLyric(songs[position].title, songs[position].artist == "Various Artists" ? "" : songs[position].artist);
            QString mod_url = songs[position].picture;
            mod_url.replace("mpic", "lpic");
            imgmgr->get(QNetworkRequest(QUrl(mod_url)));
            if (songs[position].like) {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}\nQToolButton:hover{border-image: url(:/img/unlike.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
            }
            else {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}\nQToolButton:hover{border-image: url(:/img/like.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
            }
            ui->lyricWidget->clear();
            qDebug() << "Current playing: " << songs[position].artist << ":" << songs[position].title;
        });
        if (player.state() != QMediaPlayer::PlayingState) {
            setArtistName(songs[0].artist);
            setSongName(songs[0].title);
            QString mod_url = songs[0].picture;
            setAlbumName(songs[0].albumtitle);
            lyric_getter->getLyric(songs[0].title, songs[0].artist == "Various Artists" ? "" : songs[0].artist);
            mod_url.replace("mpic", "lpic");
            ui->lyricWidget->clear();
            imgmgr->get(QNetworkRequest(QUrl(mod_url)));
            player.play();
            if (songs[0].like) {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}\nQToolButton:hover{border-image: url(:/img/unlike.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
            }
            else {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}\nQToolButton:hover{border-image: url(:/img/like.png);}\nQToolButton:clicked{border-image: url(:/img/like_disabled.png);}\nQToolButton:disabled{border-image: url(:/img/like_disabled.png);}");
            }
            qDebug() << "Current playing:" << songs[0].artist << ":" << songs[0].title;
        }
    });

    connect(static_cast<mainwidget *>(this->parentWidget())->channelWidget(), &ChannelWidget::channelChanged,
            [this] (qint32 channel) {
        this->channel = channel;
        doubanfm->getNewPlayList(channel);
    });
    connect(ui->volumeTime, &VolumeTimePanel::volumeChanged, [this] (int value) {
        player.setVolume(value);
        volume = value;
    });

    if (channel == -3) {
        if (!doubanfm->getUser())
            channel = 1;
        doubanfm->getNewPlayList(channel);
    }

    if (doubanfm->hasLogin())
        ui->userLogin->setText(doubanfm->getUser()->user_name);

    lyric_getter = new LyricGetter(this);
    connect(lyric_getter, &LyricGetter::gotLyric, [this] (const QLyricList& lyric) {
        ui->lyricWidget->setLyric(lyric);
    });

    ui->lyricWidget->setVisible(false);
    connect(ui->albumImg, &AlbumImage::clicked, [this] () {
        if (!ui->lyricWidget->isVisible())
            emit ui->lyricWidgetTriggerLeft->enter();
        else
            emit ui->lyricWidgetTriggerRight->enter();
    });

    connect(ui->channelWidgetTrigger, &ChannelWidgetTrigger::enter,
            [this] () {
        if (!static_cast<mainwidget *>(this->parentWidget())->isChannelWidgetShowing())
            static_cast<mainwidget *>(this->parentWidget())->animShowChannelWidget();

    });
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
    channel = settings.value("channel", 1).toInt();
    player.setVolume(settings.value("volume", 100).toInt());
    volume = player.volume();
    settings.endGroup();
    settings.beginGroup("User");
    std::shared_ptr<DoubanUser> user(new DoubanUser());
    user->email = settings.value("email", "").toString();
    user->expire = settings.value("expire", "").toString();
    user->password = settings.value("password", "").toString();
    user->token = settings.value("token", "").toString();
    user->user_id = settings.value("user_id", "").toString();
    user->user_name = settings.value("user_name", "").toString();
    if (user->email.size() && user->expire.size()
            && user->password.size() && user->token.size()
            && user->user_id.size() && user->user_name.size())
        doubanfm->setUser(user);
    settings.endGroup();
}

void ControlPanel::saveConfig() {
    QSettings settings("QDoubanFM", "QDoubanFM");
    settings.beginGroup("General");
    settings.setValue("channel", channel);
    settings.setValue("volume", volume);
    settings.endGroup();
    std::shared_ptr<DoubanUser> user = doubanfm->getUser();
    if (!user) user.reset(new DoubanUser);
    settings.beginGroup("User");
    settings.setValue("email", user->email);
    settings.setValue("expire", user->expire);
    settings.setValue("password", user->password);
    settings.setValue("token", user->token);
    settings.setValue("user_id", user->user_id);
    settings.setValue("user_name", user->user_name);
    settings.endGroup();
    settings.sync();
}

void ControlPanel::on_nextButton_clicked()
{
    if (static_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        static_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    int sindex = player.playlist()->currentIndex();
    doubanfm->skipSong(songs[sindex].sid, channel);
    ui->nextButton->setEnabled(false);
    player.setPosition(player.duration());
    ui->seeker->setValue(0);
}

void ControlPanel::on_pauseButton_clicked()
{
    if (static_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        static_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    if (isPaused) this->play();
    else this->pause();
    isPaused = !isPaused;
    static_cast<mainwidget *>(this->parentWidget())->pauseMask()->setVisible(true);
}

void ControlPanel::on_likeButton_clicked()
{
    if (static_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        static_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    int sindex = player.playlist()->currentIndex();
    doubanfm->rateSong(songs[sindex].sid, channel, !songs[sindex].like);
    ui->likeButton->setEnabled(false);
}

void ControlPanel::on_trashButton_clicked()
{
    if (static_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        static_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
        return;
    }
    int sindex = player.playlist()->currentIndex();
    doubanfm->byeSong(songs[sindex].sid, channel);
    ui->trashButton->setEnabled(false);
    player.setPosition(player.duration());
    ui->seeker->setValue(0);
    if (isPaused) {
        player.play();
        isPaused = false;
    }
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
    QPropertyAnimation *fadein = new QPropertyAnimation(&player, "volume");
    fadein->setDuration(1000);
    fadein->setStartValue(player.volume());
    player.play();
    fadein->setEndValue(volume);
    fadein->start(QPropertyAnimation::DeleteWhenStopped);
    isPaused = false;
}

void ControlPanel::pause() {
    QPropertyAnimation *fadeout = new QPropertyAnimation(&player, "volume");
    fadeout->setDuration(1000);
    fadeout->setStartValue(player.volume());
    fadeout->setEndValue(0);
    connect(fadeout, &QPropertyAnimation::finished, [this] () {
        player.pause();
    });
    fadeout->start(QPropertyAnimation::DeleteWhenStopped);
    isPaused = true;
}

void ControlPanel::enterEvent(QEvent *ev) {
    static_cast<mainwidget *>(this->parentWidget())->animHideChannelWidget(true);
}
