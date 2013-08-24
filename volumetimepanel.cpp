#include "volumetimepanel.h"
#include "ui_volumetimepanel.h"
#include <QTime>
#include <QPropertyAnimation>
#include <QDebug>
#include <QSettings>

VolumeTimePanel::VolumeTimePanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeTimePanel)
{
    ui->setupUi(this);
    ui->volume->setVisible(false);
    connect(ui->volume, &QSlider::valueChanged, [this] (int value) {emit volumeChanged(value);});
    QSettings settings("QDoubanFM", "QDoubanFM");
    settings.beginGroup("General");
    ui->volume->setValue(settings.value("volume", 100).toInt());
    settings.endGroup();
}

VolumeTimePanel::~VolumeTimePanel()
{
    delete ui;
}

void VolumeTimePanel::setTick(qint64 tick) {
    QTime displayTime(0, (tick / 60000) % 60, (tick / 1000) % 60);
    ui->tick->setText(//QString("<font color='black'>") +
                             displayTime.toString("m:ss")); //+ QString("</font>"));
}

void VolumeTimePanel::enterEvent(QEvent *) {
    QPropertyAnimation *anim = new QPropertyAnimation(ui->volumeImg, "geometry");
    anim->setDuration(300);
    anim->setStartValue(ui->volumeImg->geometry());
    QRect endval(0, ui->volumeImg->geometry().y(),
                 ui->volumeImg->geometry().width(),
                 ui->volumeImg->geometry().height());
    anim->setEndValue(endval);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    ui->tick->setVisible(false);
    connect(anim, &QPropertyAnimation::finished, [this] () {
        ui->volume->setVisible(true);
    });
    anim->start(QPropertyAnimation::DeleteWhenStopped);
}

void VolumeTimePanel::leaveEvent(QEvent *) {
    QPropertyAnimation *anim = new QPropertyAnimation(ui->volumeImg, "geometry");
    anim->setDuration(300);
    anim->setStartValue(ui->volumeImg->geometry());
    QRect endval(this->width() - ui->volumeImg->width(),
                 ui->volumeImg->geometry().y(),
                 ui->volumeImg->geometry().width(),
                 ui->volumeImg->geometry().height());
    anim->setEndValue(endval);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    ui->volume->setVisible(false);
    connect(anim, &QPropertyAnimation::finished, [this] () {
        ui->tick->setVisible(true);
    });
    anim->start(QPropertyAnimation::DeleteWhenStopped);
}
