#ifndef DOUBANCHANNELMANAGER_H
#define DOUBANCHANNELMANAGER_H

#include <QObject>

#include <libs/douban_types.h>
class DoubanFM;

class DoubanChannelManager : public QObject
{
    Q_OBJECT
public:
    static DoubanChannelManager& getInstance();

    DoubanChannel currentChannel() const;
    DoubanChannel& currentChannel();

signals:
    void reloadChannels();
    void channelChanged(const DoubanChannel& channel);

public slots:
    void reload();

private:
    explicit DoubanChannelManager(QObject *parent = nullptr);

    QList<DoubanChannel> channels;
    QList<DoubanChannel>::size_type curIndex;

    DoubanFM &doubanfm;
};

#endif // DOUBANCHANNELMANAGER_H
