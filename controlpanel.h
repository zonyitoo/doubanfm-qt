#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include "doubanfm.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QNetworkAccessManager>
#include "lyricgetter.h"
#include "libnotify-qt/Notification.h"

namespace Ui {
class ControlPanel;
}

class ControlPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit ControlPanel(QWidget *parent = 0);
    ~ControlPanel();

    void enterEvent(QEvent *ev);

private:
    Ui::ControlPanel *ui;
    DoubanFM *doubanfm;
    qint32 channel;
    QList<DoubanChannel> channels;
    QList<DoubanFMSong> songs;
    QMediaPlayer player;

    QNetworkAccessManager *imgmgr;
    Notification *notify;

    void loadConfig();
    void saveConfig();

    bool isPaused;
    LyricGetter *lyric_getter;

    int volume;

public slots:
    void pause();
    void play();

    void on_nextButton_clicked();
    void on_pauseButton_clicked();
    void on_likeButton_clicked();
    void on_trashButton_clicked();
    void on_userLogin_clicked();

private slots:
    void setSongName(const QString &name);
    void setArtistName(const QString &name);
    void setAlbumName(const QString &name);
};

#endif // CONTROLPANEL_H
