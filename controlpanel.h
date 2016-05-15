#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include "libs/doubanfm.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QNetworkAccessManager>
#include "libs/doubanplayer.h"
#include "libs/lyricgetter.h"
#include "settingdialog.h"

namespace Ui {
class ControlPanel;
}

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = 0);
    ~ControlPanel();

signals:
    void openChannelPanel();
    void closeChannelPanel();
    void openLyricPanel();
    void closeLyricPanel();

private:
    Ui::ControlPanel *ui;
    DoubanFM& doubanfm;
    //qint32 channel;
    //QList<DoubanChannel> channels;
    //QList<DoubanFMSong> songs;
    //QMediaPlayer player;
    DoubanPlayer& player;

    QNetworkAccessManager *imgmgr;
    //Notification *notify;

    void loadConfig();
    void saveConfig();

    //bool isPaused;
    LyricGetter *lyricGetter;

    //int volume;

    SettingDialog *settingDialog;

public slots:
    void pause();
    void play();

    void on_nextButton_clicked();
    void on_pauseButton_clicked();
    void on_likeButton_clicked();
    void on_trashButton_clicked();

private slots:
    void setSongName(const QString &name);
    void setArtistName(const QString &name);
    void setAlbumName(const QString &name);
    void on_settingButton_clicked();
    void on_lyricButton_clicked(bool checked);
    void on_channelButton_clicked(bool checked);
};

#endif // CONTROLPANEL_H
