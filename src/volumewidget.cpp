#include "volumewidget.h"
#include "ui_volumewidget.h"
#include <QPropertyAnimation>
#include <QDebug>
#include <phonon/AudioOutput>

VolumeWidget::VolumeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeWidget)
{
    ui->setupUi(this);

    QRect rect(this->geometry());
    rect.setWidth(0);
    rect.setHeight(63);
    ui->movingWidget->setGeometry(rect);

    _isShowing = false;
}

VolumeWidget::~VolumeWidget()
{
    delete ui;
}

void VolumeWidget::leaveEvent(QEvent *event) {
    animHide();
}

void VolumeWidget::animShow() {
    _isShowing = true;
    QPropertyAnimation *anim = new QPropertyAnimation(ui->movingWidget, "geometry");
    anim->setStartValue(ui->movingWidget->geometry());
    QRect endRect(ui->movingWidget->geometry().x(),
                  ui->movingWidget->geometry().y(),
                  this->width(),
                  ui->movingWidget->geometry().height());
    anim->setEndValue(endRect);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void VolumeWidget::animHide() {
    _isShowing = false;
    QPropertyAnimation *anim = new QPropertyAnimation(ui->movingWidget, "geometry");
    anim->setStartValue(ui->movingWidget->geometry());
    QRect endRect(ui->movingWidget->geometry().x(),
                  ui->movingWidget->geometry().y(),
                  0,
                  ui->movingWidget->geometry().height());
    anim->setEndValue(endRect);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

bool VolumeWidget::isShowing() {
    return _isShowing;
}

void VolumeWidget::on_volumeSlider_valueChanged(int value) {
    emit this->volumeChanged(value / 100.0);
}

void VolumeWidget::setVolume(qreal newvol) {
    ui->volumeSlider->setValue(newvol * 100);
}
