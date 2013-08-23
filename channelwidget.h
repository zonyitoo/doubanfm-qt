#ifndef CHANNELWIDGET_H
#define CHANNELWIDGET_H

#include <QWidget>
#include "douban_types.h"
#include <QList>
#include <QLabel>
#include "doubanfm.h"
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

public slots:
    void setChannels(const QList<DoubanChannel>& channels);

signals:
    void channelChanged(qint32 channel);

private slots:
    void on_nextButton_clicked();

    void on_prevButton_clicked();

private:
    Ui::ChannelWidget *ui;

    QObjectList labels;
    DoubanFM *doubanfm;
    QList<DoubanChannel> channels;
    qint32 channel;
    QTimer *timer;
};

#endif // CHANNELWIDGET_H
