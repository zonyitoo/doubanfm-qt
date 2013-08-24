#include "horizontalslider.h"
#include <QRect>
#include <QPropertyAnimation>
#include <QDebug>

HorizontalSlider::HorizontalSlider(QWidget *parent) :
    QWidget(parent)
{
    container = new QWidget(this);
    hbox = new QHBoxLayout(container);
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->setContentsMargins(0, 0, 0, 0);
    container->setLayout(hbox);
    container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    curIndex = -1;
}

HorizontalSlider::~HorizontalSlider() {
    delete hbox;
}

void HorizontalSlider::setChildren(const QObjectList& list) {
    int accu = 0;
    for (QObject const *object : list) {
        hbox->addWidget((QWidget *) object);
        widths.append(((QWidget *) object)->width());
        accu += ((QWidget *) object)->width();
    }
    container->setMaximumWidth(accu);
    container->setMinimumWidth(accu);
    container->setMaximumHeight(this->height());
    container->setMinimumHeight(this->height());
    curIndex = 0;
    container->move(QPoint(this->width() / 2 - widths[0] / 2, 0));
    refresh();
}

void HorizontalSlider::scrollToIndex(int index) {
    if (index == curIndex) return;
    if (index > numberOfChildren() - 1) return;

    QPropertyAnimation *anim = new QPropertyAnimation(container, "geometry");
    anim->setDuration(300);
    anim->setStartValue(container->geometry());
    int accuwidth = 0;
    for (int i = 0; i < index; ++ i) accuwidth += widths[i];
    accuwidth += widths[index] / 2 - this->width() / 2;
    QRect endval(-accuwidth,
                            container->geometry().y(),
                            container->geometry().width(),
                            container->geometry().height()
                          );
    anim->setEndValue(endval);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QPropertyAnimation::finished, [=] () {
        this->curIndex = index;
        emit scrollFinished();
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

int HorizontalSlider::numberOfChildren() {
    return widths.size();
}

int HorizontalSlider::currentIndex() {
    return curIndex;
}

void HorizontalSlider::refresh() {

}
