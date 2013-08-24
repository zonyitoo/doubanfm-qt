#include "titlewidget.h"
#include "ui_titlewidget.h"
#include <QMouseEvent>
#include <QPoint>
#include <QDebug>
#include "mainwidget.h"
#include <QPropertyAnimation>
#include <QSettings>
#include "doubanfm.h"

TitleWidget::TitleWidget(QWidget *parent) :
    QWidget(parent), parent(parent),
    ui(new Ui::TitleWidget),
    _isVolumeShowing(false),
    _isLoginPanelOpen(false)
{
    ui->setupUi(this);
    ui->volume->setVisible(false);
    QPalette bpa = ui->userLogin->palette();
    bpa.setColor(QPalette::ButtonText, Qt::white);
    ui->userLogin->setPalette(bpa);
    connect(ui->volume, &QSlider::valueChanged, [this] (int value) {
        emit this->volumeChanged(value);
    });
    connect(DoubanFM::getInstance(), &DoubanFM::loginSucceed, [this] (std::shared_ptr<DoubanUser> user) {
        ui->userLogin->setText(user->user_name);
    });
    connect(ui->minimizeButton, SIGNAL(clicked()), this->parentWidget(), SLOT(showMinimized()));
    connect(ui->exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QSettings settings("QDoubanFM", "QDoubanFM");
    settings.beginGroup("General");
    ui->volume->setValue(settings.value("volume", 100).toInt());
    settings.endGroup();
    settings.beginGroup("User");
    ui->userLogin->setText(settings.value("user_name", "未登录").toString());
    settings.endGroup();
}

TitleWidget::~TitleWidget()
{
    delete ui;
}

void TitleWidget::mousePressEvent(QMouseEvent *event) {
    QPoint wpos = parent->pos();
    QPoint mousepos = event->globalPos();
    this->dpos = mousepos - wpos;
}

void TitleWidget::mouseMoveEvent(QMouseEvent *event) {
    parent->move(event->globalPos() - this->dpos);
}

void TitleWidget::on_volumeButton_clicked()
{
    if (_isVolumeShowing)
        animHideVolume();
    else
        animShowVolume();
    _isVolumeShowing = !_isVolumeShowing;
}

void TitleWidget::animShowVolume() {
    QPropertyAnimation *anim = new QPropertyAnimation(ui->volumeButton, "geometry");
    anim->setDuration(400);
    anim->setStartValue(ui->volumeButton->geometry());
    QRect endval(ui->volumeButton->geometry());
    endval.setX(ui->volume->geometry().x() - ui->volumeButton->width() - 10);
    anim->setEndValue(endval);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QPropertyAnimation::finished, [this] () {
        ui->volume->setVisible(true);
        _isVolumeShowing = true;
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void TitleWidget::animHideVolume() {
    QPropertyAnimation *anim = new QPropertyAnimation(ui->volumeButton, "geometry");
    anim->setDuration(400);
    anim->setStartValue(ui->volumeButton->geometry());
    QRect endval(ui->volumeButton->geometry());
    endval.setX(496);
    anim->setEndValue(endval);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    ui->volume->setVisible(false);
    connect(anim, &QPropertyAnimation::finished, [this] () {
        _isVolumeShowing = false;
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void TitleWidget::on_userLogin_clicked()
{
    LoginPanel *loginPanel = dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel();
    if (_isLoginPanelOpen)
        loginPanel->animHide();
    else
        loginPanel->animShow();
    _isLoginPanelOpen = !_isLoginPanelOpen;
}
