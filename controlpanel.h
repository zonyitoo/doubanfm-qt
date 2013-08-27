#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include "doubanfm.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QNetworkAccessManager>
#include "lyricgetter.h"

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

    void loadConfig();
    void saveConfig();

    bool isPaused;
    LyricGetter *lyric_getter;

public slots:
    void pause();
    void play();

private slots:
    void setSongName(const QString &name);
    void setArtistName(const QString &name);
    void setAlbumName(const QString &name);

    void on_nextButton_clicked();
    void on_pauseButton_clicked();
    void on_likeButton_clicked();
    void on_trashButton_clicked();
    void on_userLogin_clicked();
};

#endif // CONTROLPANEL_H
