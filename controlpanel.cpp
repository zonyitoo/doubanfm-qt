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
    channel(0), isPaused(false)
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
        setAlbumImage(image);
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
            QString mod_url = songs[position].picture;
            mod_url.replace("mpic", "lpic");
            imgmgr->get(QNetworkRequest(QUrl(mod_url)));
            if (songs[position].like) {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}\nQToolButton:hover{border-image: url(:/img/unlike.png);}");
            }
            else {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}\nQToolButton:hover{border-image: url(:/img/like.png);}");
            }
        });
        if (player.state() != QMediaPlayer::PlayingState) {
            setArtistName(songs[0].artist);
            setSongName(songs[0].title);
            setAlbumName(songs[0].albumtitle);
            QString mod_url = songs[0].picture;
            mod_url.replace("mpic", "lpic");
            imgmgr->get(QNetworkRequest(QUrl(mod_url)));
            player.play();
            qDebug() << "Current playing: " << songs[0].artist << ":" << songs[0].title;
        }
    });

    connect(&player, SIGNAL(positionChanged(qint64)), ui->volumeTime, SLOT(setTick(qint64)));
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
        int index = player.playlist()->currentIndex();
        if (songs[index].like) {
            ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}\nQToolButton:hover{border-image: url(:/img/like.png);}");
        }
        else {
            ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}\nQToolButton:hover{border-image: url(:/img/unlike.png);}");
        }
        songs[index].like = !songs[index].like;
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
            QString mod_url = songs[position].picture;
            mod_url.replace("mpic", "lpic");
            imgmgr->get(QNetworkRequest(QUrl(mod_url)));
            if (songs[position].like) {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/like.png);}\nQToolButton:hover{border-image: url(:/img/unlike.png);}");
            }
            else {
                ui->likeButton->setStyleSheet("QToolButton{border-image: url(:/img/unlike.png);}\nQToolButton:hover{border-image: url(:/img/like.png);}");
            }
            qDebug() << "Current playing: " << songs[position].artist << ":" << songs[position].title;
        });
        if (player.state() != QMediaPlayer::PlayingState) {
            setArtistName(songs[0].artist);
            setSongName(songs[0].title);
            QString mod_url = songs[0].picture;
            setAlbumName(songs[0].albumtitle);
            mod_url.replace("mpic", "lpic");
            imgmgr->get(QNetworkRequest(QUrl(mod_url)));
            player.play();
            qDebug() << "Current playing:" << songs[0].artist << ":" << songs[0].title;
        }
    });

    connect(dynamic_cast<mainwidget *>(this->parentWidget())->channelWidget(), &ChannelWidget::channelChanged,
            [this] (qint32 channel) {
        this->channel = channel;
        doubanfm->getNewPlayList(channel);
    });
    connect(ui->volumeTime, &VolumeTimePanel::volumeChanged, [this] (int value) {
        player.setVolume(value);
    });

    if (channel == -3) {
        if (!doubanfm->getUser())
            channel = 1;
        doubanfm->getNewPlayList(channel);
    }

    if (doubanfm->hasLogin())
        ui->userLogin->setText(doubanfm->getUser()->user_name);
}

ControlPanel::~ControlPanel()
{
    delete ui;
    saveConfig();
}

void ControlPanel::setAlbumImage(const QImage &src) {
    static const QSize picsize(136, 136);

    QImage image = src.scaled(picsize.width(), picsize.height(), Qt::KeepAspectRatioByExpanding)
            .copy(0, 0, picsize.width(), picsize.height());

    /*
    QLinearGradient gardient(QPoint(0, 0), QPoint(0, image.height()));
    gardient.setColorAt(0, Qt::white);
    gardient.setColorAt(0.1, Qt::black);

    QImage mask(picsize, image.format());
    QPainter painter(&mask);
    painter.fillRect(mask.rect(), gardient);
    painter.end();

    QImage reflection = image.mirrored();
    reflection.setAlphaChannel(mask);

    QImage imgdraw(QSize(ui->albumImg->width(), ui->albumImg->height()), QImage::Format_ARGB32_Premultiplied);
    QPainter album_painter(&imgdraw);

    album_painter.drawImage(0, 0, image);
    //album_painter.setOpacity(0.5);
    album_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    album_painter.drawImage(0, image.height(), reflection);
    album_painter.end();
*/
    QPixmap mirror(image.width(), ui->albumImg->height() - image.height());
    mirror.fill(Qt::transparent);
    QPainter mirrorp(&mirror);
    QLinearGradient linearGrad(QPoint(mirror.width(), 0), QPoint(mirror.width(), mirror.height() / 2));
    linearGrad.setColorAt(1, QColor(255,255,255,0));
    linearGrad.setColorAt(0.8, QColor(255,255,255,20));
    linearGrad.setColorAt(0, QColor(255,255,255,200));
    mirrorp.setBrush(linearGrad);
    mirrorp.fillRect(0, 0, mirror.width(), mirror.height(), QBrush(linearGrad));
    mirrorp.setCompositionMode(QPainter::CompositionMode_SourceIn);
    mirrorp.drawPixmap(0, 0, QPixmap::fromImage(image.copy(0, 2 * image.height() - ui->albumImg->height(), image.width(), ui->albumImg->height() - image.height()).mirrored(false, true)));
    mirrorp.end();

    QImage imgdraw(QSize(ui->albumImg->width(), ui->albumImg->height()), QImage::Format_ARGB32_Premultiplied);
    imgdraw.fill(Qt::transparent);
    QPainter album_painter(&imgdraw);
    album_painter.drawImage(0, 0, image);
    //album_painter.setOpacity(0.8);
    album_painter.drawPixmap(0, image.height(), mirror);
    album_painter.end();
    QPixmap empty(ui->albumImg->size());
    empty.fill(Qt::transparent);
    ui->albumImg->setPixmap(empty);
    ui->albumImg->setPixmap(QPixmap::fromImage(imgdraw));
}

void ControlPanel::setSongName(const QString &name) {
    ui->songName->setText(QString("<font color='#04aaa1'>")
                          + name + QString("</font>"));
}

void ControlPanel::setArtistName(const QString &name) {
    ui->artist->setText(QString("<font color='grey'>")
                          + name + QString("</font>"));
}

void ControlPanel::setTick(qint64 tick) {
    QTime displayTime(0, (tick / 60000) % 60, (tick / 1000) % 60);
    //ui->time->setText(QString("<font color='black'>") +
    //                         displayTime.toString("m:ss") + QString("</font>"));
}

void ControlPanel::loadConfig() {
    QSettings settings("QDoubanFM", "QDoubanFM");

    settings.beginGroup("General");
    channel = settings.value("channel", 1).toInt();
    player.setVolume(settings.value("volume", 100).toInt());
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
    settings.setValue("volume", player.volume());
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
    if (dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    int sindex = player.playlist()->currentIndex();
    doubanfm->skipSong(songs[sindex].sid, channel);
    ui->nextButton->setEnabled(false);
    player.setPosition(player.duration());
    ui->seeker->setValue(0);
}

void ControlPanel::on_pauseButton_clicked()
{
    if (dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    if (isPaused) player.play();
    else player.pause();
    isPaused = !isPaused;
    dynamic_cast<mainwidget *>(this->parentWidget())->pauseMask()->setVisible(true);
}

void ControlPanel::on_likeButton_clicked()
{
    if (dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
    }
    int sindex = player.playlist()->currentIndex();
    doubanfm->rateSong(songs[sindex].sid, channel, !songs[sindex].like);
    ui->likeButton->setEnabled(false);
}

void ControlPanel::on_trashButton_clicked()
{
    if (dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel()->isShowing()) {
        dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel()->animHide();
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
    LoginPanel *loginPanel = dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel();
    if (!loginPanel->isShowing())
        loginPanel->animShow();
    else
        loginPanel->animHide();
}

void ControlPanel::setAlbumName(const QString &name) {
    ui->album->setText(QString("<font color=grey>&lt; ") + name + QString(" &gt;</font>"));
}

void ControlPanel::play() {
    player.play();
    isPaused = false;
}

void ControlPanel::pause() {
    player.pause();
    isPaused = true;
}
