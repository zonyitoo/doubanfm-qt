#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>
#include <QLabel>
#include "douban_types.h"
#include "douban.h"

namespace Ui {
class ChannelWidget;
}

class ChannelWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ChannelWidget(QWidget *parent = 0);
    ~ChannelWidget();

    void refreshChannels();

    int getInvisibleHeight();
    void setChannelId(qint32 channel_id);

signals:
    void channelChanged(qint32 channel_id);
    void channelChanged(const DoubanChannel& channel);

private slots:
    void onRecvChannels(const QList<DoubanChannel>& channels);
    void onAnimFinished();
    
private:
    Ui::ChannelWidget *ui;

    Douban *_douban;

    QWidget *_channelContainer;
    QList<QLabel *> _channelLabels;
    QList<DoubanChannel> _channels;

    int curValueIndex;
    qint32 curChannelId;

    void mousePressEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // CHANNELWIDGET_H
