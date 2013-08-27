#ifndef LYRICWIDGET_H
#define LYRICWIDGET_H

#include <QWidget>
#include "qlyricparser.h"
#include <QTime>
#include <QLabel>
#include "ui_lyricwidget.h"
#include <QMouseEvent>

class LyricWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LyricWidget(QWidget *parent = 0);
    ~LyricWidget();

    void setLyric(const QLyricList& lyric);
    void clear();

    void mousePressEvent(QMouseEvent *event);

signals:
    void clicked();
    
public slots:
    void setTick(qint64 tick);
    void setTime(const QTime& time);

private:
    QWidget *animWidget;
    int curInd;
    QLyricList lyric;
    QList<QLabel *> labels;
    QList<int> heights;
    bool firstShowing;

    Ui_LyricWidget *ui;
};

#endif // LYRICWIDGET_H
