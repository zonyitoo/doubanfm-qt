#include "horizontalslider.h"
#include <QRect>
#include <QPropertyAnimation>
#include <QDebug>

HorizontalSlider::HorizontalSlider(QWidget *parent) :
    QWidget(parent), hbox(nullptr)
{
    container = new QWidget(this);
    container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    curIndex = 0;
}

HorizontalSlider::~HorizontalSlider() {
    delete container;
}

void HorizontalSlider::clear() {
    if (hbox != nullptr) {
        container->setLayout(nullptr);
        delete hbox;
    }
    hbox = new QHBoxLayout(container);
    hbox->setMargin(0);
    hbox->setSpacing(0);
    hbox->setContentsMargins(0, 0, 0, 0);
    container->setLayout(hbox);
}

void HorizontalSlider::setChildren(const QList<QWidget *>& list) {
    clear();

    items = list;
    int accu = 0;
    for (QWidget *object : list) {
        object->setParent(container);
        hbox->addWidget(object);
        widths.append(object->width());
        accu += object->width();
    }
    container->setMaximumWidth(accu);
    container->setMinimumWidth(accu);
    container->setMaximumHeight(this->height());
    container->setMinimumHeight(this->height());
    curIndex = 0;
    container->move(QPoint(this->width() / 2 - widths[0] / 2, 0));
    refresh();
}

void HorizontalSlider::addChild(QWidget *widget) {
    hbox->addWidget(widget);
    widths.append(widget->width());
    auto orig_width = container->maximumWidth();
    container->setMaximumWidth(orig_width + widget->width());
    container->setMinimumWidth(orig_width + widget->width());
}

void HorizontalSlider::scrollToIndex(int index) {
    if (index == curIndex) return;
    if (index > numberOfChildren() - 1) return;

    this->curIndex = index;

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
        emit scrollFinished();
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

int HorizontalSlider::numberOfChildren() {
    return items.size();
}

int HorizontalSlider::currentIndex() {
    return curIndex;
}

void HorizontalSlider::refresh() {

}

QWidget *HorizontalSlider::currentObject() const {
    return items[curIndex];
}

QWidget *HorizontalSlider::preObject() const {
    return items[curIndex - 1];
}

QWidget *HorizontalSlider::nextObject() const {
    return items[curIndex + 1];
}
