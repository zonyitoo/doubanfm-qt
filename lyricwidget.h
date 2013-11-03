#ifndef LYRICWIDGET_H
#define LYRICWIDGET_H

#include <QWidget>
#include "libs/qlyricparser.h"
#include <QTime>
#include <QLabel>
#include "ui_lyricwidget.h"
#include <QMouseEvent>
#include "libs/doubanfm.h"
#include "libs/lyricgetter.h"
#include "libs/douban_types.h"

class LyricWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LyricWidget(QWidget *parent = 0);
    ~LyricWidget();


    void clear();

    void mousePressEvent(QMouseEvent *event);

signals:
    void clicked();
    
public slots:
    void setTick(qint64 tick);
    void setTime(const QTime& time);
    void setLyric(const QLyricList& lyric);
    void setSong(const DoubanFMSong& song);
    void setShowing(bool);

private:
    Ui_LyricWidget *ui;
    QWidget *animWidget;
    int curInd;
    QLyricList lyric;
    QList<QLabel *> labels;
    QList<int> heights;
    bool firstShowing;
    LyricGetter *lyricGetter;
    bool isShowing;
    DoubanFMSong saveCurrentSong;
    bool haveSearchedLyric;
    qint64 saveTick;
};

#endif // LYRICWIDGET_H
