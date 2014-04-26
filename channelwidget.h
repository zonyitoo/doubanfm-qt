#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>
#include "libs/douban_types.h"
#include <QList>
#include <QLabel>
#include "libs/doubanfm.h"
#include <QTimer>

namespace Ui {
class ChannelWidget;
}

class ChannelWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ChannelWidget(QWidget *parent = 0);
    ~ChannelWidget();

    void leaveEvent(QEvent *ev);
    void wheelEvent(QWheelEvent *ev);
public slots:
    void setChannels(const QList<DoubanChannel>& channels);

signals:
    void channelChanged(qint32 channel);
    void mouseLeave();

private slots:
    void on_nextButton_clicked();

    void on_prevButton_clicked();

private:
    Ui::ChannelWidget *ui;

    DoubanFM& doubanfm;
    QList<DoubanChannel> channels;
    qint32 channel;
    QTimer *timer;
};

#endif // CHANNELWIDGET_H
