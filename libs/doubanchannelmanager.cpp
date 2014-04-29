#include "doubanchannelmanager.h"
#include "doubanfm.h"

DoubanChannelManager::DoubanChannelManager(QObject *parent) :
    QObject(parent),
    curIndex(-1),
    doubanfm(DoubanFM::getInstance())
{
    connect(&doubanfm, &DoubanFM::receivedChannels, [&] (const QList<DoubanChannel>& channels) {
        this->channels = channels;
        curIndex = 0;
    });
}

DoubanChannelManager& DoubanChannelManager::getInstance() {
    static DoubanChannelManager _INSTANCE;
    return _INSTANCE;
}

DoubanChannel DoubanChannelManager::currentChannel() const {
    return channels[curIndex];
}

DoubanChannel &DoubanChannelManager::currentChannel() {
    return channels[curIndex];
}

void DoubanChannelManager::reload() {
    channels.clear();
    curIndex = -1;

    doubanfm.getChannels();
}
