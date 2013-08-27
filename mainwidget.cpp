#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPoint>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

mainwidget::mainwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainwidget),
    _isChannelWidgetShowing(true)
{
    ui->setupUi(this);
    ui->pauseWidget->setVisible(false);

    connect(ui->pauseWidget, SIGNAL(clicked()), ui->controlWidget, SLOT(play()));
    exitShortcut = new QShortcut(QKeySequence::Close, this);
    connect(exitShortcut, SIGNAL(activated()), qApp, SLOT(quit()));
    pauseShortcut = new QShortcut(QKeySequence("Space"), this);
    connect(pauseShortcut, &QShortcut::activated, [this] () {
        bool visiable = ui->pauseWidget->isVisible();
        if (visiable)
            ui->controlWidget->play();
        else
            ui->controlWidget->pause();
        ui->pauseWidget->setVisible(!visiable);
    });

    connect(ui->channelWidget, SIGNAL(mouseLeave()), this, SLOT(animHideChannelWidget()));
    animHideChannelWidget();
}

mainwidget::~mainwidget()
{
    delete ui;
    delete exitShortcut;
    delete pauseShortcut;
}

ChannelWidget *mainwidget::channelWidget() {
    return ui->channelWidget;
}

ControlPanel *mainwidget::controlPanel() {
    return ui->controlWidget;
}

LoginPanel *mainwidget::loginPanel() {
    return ui->loginWidget;
}

PauseMask *mainwidget::pauseMask() {
    return ui->pauseWidget;
}

void mainwidget::mousePressEvent(QMouseEvent *) {
    if (this->loginPanel()->isShowing())
        this->loginPanel()->animHide();
}

void mainwidget::animHideChannelWidget() {
    if (!_isChannelWidgetShowing) return;

    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup(this);
    QPropertyAnimation *control_anim = new QPropertyAnimation(ui->controlWidget, "geometry");
    control_anim->setDuration(400);
    control_anim->setStartValue(ui->controlWidget->geometry());
    QRect endval(0, 0, ui->controlWidget->geometry().width(),
                 ui->controlWidget->geometry().height());
    control_anim->setEndValue(endval);
    control_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(control_anim);

    QPropertyAnimation *main_anim = new QPropertyAnimation(this, "geometry");
    main_anim->setDuration(400);
    main_anim->setStartValue(this->geometry());
    QRect endval2(this->geometry().x(), this->geometry().y(),
                  this->geometry().width(), this->geometry().height() - ui->channelWidget->geometry().height());
    main_anim->setEndValue(endval2);
    main_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(main_anim);

    connect(animgroup, &QParallelAnimationGroup::finished, [this] () {
        _isChannelWidgetShowing = false;
        ui->pauseWidget->setGeometry(0, 0, ui->pauseWidget->geometry().width(), this->geometry().height());
    });
    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void mainwidget::animShowChannelWidget() {
    if (_isChannelWidgetShowing) return;

    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup(this);
    QPropertyAnimation *control_anim = new QPropertyAnimation(ui->controlWidget, "geometry");
    control_anim->setDuration(400);
    control_anim->setStartValue(ui->controlWidget->geometry());
    QRect endval(0, ui->channelWidget->geometry().height(), ui->controlWidget->geometry().width(),
                 ui->controlWidget->geometry().height());
    control_anim->setEndValue(endval);
    control_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(control_anim);

    QPropertyAnimation *main_anim = new QPropertyAnimation(this, "geometry");
    main_anim->setDuration(400);
    main_anim->setStartValue(this->geometry());
    QRect endval2(this->geometry().x(), this->geometry().y(),
                  this->geometry().width(), this->geometry().height() + ui->channelWidget->geometry().height());
    main_anim->setEndValue(endval2);
    main_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(main_anim);

    connect(animgroup, &QParallelAnimationGroup::finished, [this] () {
        _isChannelWidgetShowing = true;
        ui->pauseWidget->setGeometry(0, 0, ui->pauseWidget->geometry().width(), this->geometry().height());
    });
    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

bool mainwidget::isChannelWidgetShowing() {
    return _isChannelWidgetShowing;
}
