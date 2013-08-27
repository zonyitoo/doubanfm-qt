#include "triggerarea.h"

TriggerArea::TriggerArea(QWidget *parent) :
    QWidget(parent)
{
}

void TriggerArea::enterEvent(QEvent *ev) {
    emit enter();
}

void TriggerArea::leaveEvent(QEvent *ev) {
    emit leave();
}
