#ifndef CHANNELWIDGETTRIGGER_H
#define CHANNELWIDGETTRIGGER_H

#include "triggerarea.h"

class ChannelWidgetTrigger : public TriggerArea {
public:
    explicit ChannelWidgetTrigger(QWidget *parent)
        : TriggerArea(parent) {}
};

#endif // CHANNELWIDGETTRIGGER_H
