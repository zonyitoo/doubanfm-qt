#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPoint>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

mainwidget::mainwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainwidget),
    _isChannelWidgetShowing(false)
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

void mainwidget::animHideChannelWidget(bool immediately) {
    if (!_isChannelWidgetShowing && !immediately) return;

    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup(this);
    QPropertyAnimation *control_anim = new QPropertyAnimation(ui->controlWidget, "pos");
    control_anim->setDuration(400);
    control_anim->setStartValue(ui->controlWidget->pos());
    QPoint endpos = ui->controlWidget->pos();
    endpos.setY(0);
    control_anim->setEndValue(endpos);
    control_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(control_anim);

    QPropertyAnimation *main_anim = new QPropertyAnimation(this, "geometry");
    main_anim->setDuration(400);
    main_anim->setStartValue(this->geometry());
    QRect endval2 = this->geometry();
    endval2.setHeight(ui->controlWidget->geometry().height());
    main_anim->setEndValue(endval2);
    main_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(main_anim);

    connect(animgroup, &QParallelAnimationGroup::finished, [this] () {
        _isChannelWidgetShowing = false;
        QRect pauseGeo = ui->pauseWidget->geometry();
        pauseGeo.setHeight(this->geometry().height());
        ui->pauseWidget->setGeometry(pauseGeo);
        this->setMaximumHeight(pauseGeo.height());
    });
    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void mainwidget::animShowChannelWidget() {
    if (_isChannelWidgetShowing) return;

    this->setMaximumHeight(this->controlPanel()->geometry().height()
                           + ui->channelWidget->geometry().height());

    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup(this);
    QPropertyAnimation *control_anim = new QPropertyAnimation(ui->controlWidget, "pos");
    control_anim->setDuration(400);
    control_anim->setStartValue(ui->controlWidget->pos());
    QPoint endpos = ui->controlWidget->pos();
    endpos.setY(ui->channelWidget->geometry().height());
    control_anim->setEndValue(endpos);
    control_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(control_anim);

    QPropertyAnimation *main_anim = new QPropertyAnimation(this, "geometry");
    main_anim->setDuration(400);
    main_anim->setStartValue(this->geometry());
    QRect endval2 = this->geometry();
    endval2.setHeight(endval2.height() + ui->channelWidget->geometry().height());
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
