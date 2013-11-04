#include "triggerarea.h"

TriggerArea::TriggerArea(QWidget *parent) :
    QWidget(parent)
{
}

void TriggerArea::enterEvent(QEvent *) {
    emit enter();
}

void TriggerArea::leaveEvent(QEvent *) {
    emit leave();
}
