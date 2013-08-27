#ifndef LYRICWIDGETTRIGGER_H
#define LYRICWIDGETTRIGGER_H

#include "triggerarea.h"

class LyricWidgetTriggerLeft : public TriggerArea {
public:
    explicit LyricWidgetTriggerLeft(QWidget *parent)
        : TriggerArea(parent) {}
};

class LyricWidgetTriggerRight : public TriggerArea {
public:
    explicit LyricWidgetTriggerRight(QWidget *parent)
        : TriggerArea(parent) {}
};

#endif // LYRICWIDGETTRIGGER_H
