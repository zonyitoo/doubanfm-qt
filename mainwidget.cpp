#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "libs/doubanplayer.h"
#include <QPoint>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QMenu>


MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    _isChannelWidgetShowing(false),
    _isAnimStarted(false),
    _isLyricWidgetShowing(false),
    topBorder(new QLabel(this)),
    bottomBorder(new QLabel(this)),
    systemTrayIcon(nullptr)
{
    ui->setupUi(this);
    ui->lyricWidget->lower();
    ui->channelWidget->raise();
    ui->controlWidget->raise();

    // Configure borders
    topBorder->setMaximumSize(this->width(), 5);
    topBorder->setMinimumSize(this->width(), 5);
    topBorder->setStyleSheet("background: qlineargradient(x1: 0, y1: 1, x2: 0, y2: 0, stop: 0 rgba(0,0,0,80), stop: 1 rgba(0,0,0,0));");
    topBorder->raise();
    topBorder->move(0, -topBorder->height());
    bottomBorder->setMaximumSize(this->width(), 10);
    bottomBorder->setMinimumSize(this->width(), 10);
    bottomBorder->setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(0,0,0,120), stop: 1 rgba(0,0,0,0));");
    bottomBorder->raise();
    bottomBorder->move(0, ui->controlWidget->height());

    ui->pauseWidget->raise();
    ui->pauseWidget->setVisible(false);

    connect(ui->pauseWidget, SIGNAL(clicked()), ui->controlWidget, SLOT(play()));
    exitShortcut = new QShortcut(QKeySequence("Ctrl+Q"), this);
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
    nextShortcut = new QShortcut(QKeySequence("S"), this);
    connect(nextShortcut, SIGNAL(activated()), ui->controlWidget, SLOT(on_nextButton_clicked()));
    deleteShortcut = new QShortcut(QKeySequence("D"), this);
    connect(deleteShortcut, SIGNAL(activated()), ui->controlWidget, SLOT(on_trashButton_clicked()));
    likeShortcut = new QShortcut(QKeySequence("F"), this);
    connect(likeShortcut, SIGNAL(activated()), ui->controlWidget, SLOT(on_likeButton_clicked()));
    hideShortcut = new QShortcut(QKeySequence("Ctrl+W"), this);
    connect(hideShortcut, SIGNAL(activated()), this, SLOT(hide()));

    connect(ui->controlWidget, SIGNAL(openChannelPanel()), this, SLOT(animShowChannelWidget()));
    connect(ui->controlWidget, SIGNAL(closeChannelPanel()), this, SLOT(animHideChannelWidget()));

    connect(ui->controlWidget, SIGNAL(openLyricPanel()), this, SLOT(animShowLyricWidget()));
    connect(ui->controlWidget, SIGNAL(closeLyricPanel()), this, SLOT(animHideLyricWidget()));

    // FIXME: Ubuntu doesn't support QSystemTrayIcon
    // Following code will cause confusing behaviour
#ifdef WITH_SYSTEM_TRAY_ICON
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        systemTrayIcon = new QSystemTrayIcon(QIcon("://icon.png"), this);
        connect(systemTrayIcon, &QSystemTrayIcon::activated, [&] () {
            if (this->isHidden()) {
                this->show();
                this->raise();
                this->activateWindow();
            } else {
                if (this->isActiveWindow()) {
                    this->hide();
                } else {
                    this->raise();
                    this->activateWindow();
                }
            }
        });
        QMenu *trayMenu = new QMenu(this);
        auto _openAction = trayMenu->addAction(tr("Open main window"));
        trayMenu->addSeparator();
        auto _closeAction = trayMenu->addAction(tr("Exit"));
        connect(trayMenu, &QMenu::triggered, [this, _openAction, _closeAction] (QAction *action) {
            if (action == _openAction)
                this->show();
            else if (action == _closeAction)
                qApp->quit();
        });
        systemTrayIcon->setContextMenu(trayMenu);
        systemTrayIcon->show();
    }
#endif

}

MainWidget::~MainWidget()
{
    if (systemTrayIcon) {
        systemTrayIcon->hide();
        systemTrayIcon->deleteLater();
    }
    delete ui;
    delete exitShortcut;
    delete hideShortcut;
    delete pauseShortcut;
    delete topBorder;
    delete bottomBorder;
}

void MainWidget::mousePressEvent(QMouseEvent *) {

}

void MainWidget::animHideChannelWidget() {
    if (!isChannelWidgetShowing() || isAnimationStarted()) return;
    this->animStart();
    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup(this);
    if (this->height() == ui->channelWidget->height() + ui->controlWidget->height()) {
        this->setMinimumHeight(ui->controlWidget->height());

        QPropertyAnimation *main_anim = new QPropertyAnimation(this, "geometry");
        main_anim->setDuration(400);
        main_anim->setStartValue(this->geometry());
        QRect endval2 = this->geometry();
        endval2.setHeight(endval2.height() - ui->channelWidget->height());
        main_anim->setEndValue(endval2);
        main_anim->setEasingCurve(QEasingCurve::OutCubic);

        animgroup->addAnimation(main_anim);
    }

    QPropertyAnimation *control_anim = new QPropertyAnimation(ui->controlWidget, "pos");
    control_anim->setDuration(400);
    control_anim->setStartValue(ui->controlWidget->pos());
    QPoint endpos = ui->controlWidget->pos();
    endpos.setY(0);
    control_anim->setEndValue(endpos);
    control_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(control_anim);

    QPropertyAnimation *topborder_anim = new QPropertyAnimation(this->topBorder, "pos");
    topborder_anim->setDuration(400);
    QPoint topborder_pos = topBorder->pos();
    topborder_anim->setStartValue(topborder_pos);
    topborder_pos.setY(-topBorder->height());
    topborder_anim->setEndValue(topborder_pos);
    topborder_anim->setEasingCurve(QEasingCurve::OutCubic);
    animgroup->addAnimation(topborder_anim);

    QPropertyAnimation *bottomborder_anim = new QPropertyAnimation(this->bottomBorder, "pos");
    bottomborder_anim->setDuration(400);
    QPoint bottomborder_pos = bottomBorder->pos();
    bottomborder_anim->setStartValue(bottomborder_pos);
    bottomborder_pos.setY(ui->controlWidget->height());
    bottomborder_anim->setEndValue(bottomborder_pos);
    bottomborder_anim->setEasingCurve(QEasingCurve::OutCubic);
    animgroup->addAnimation(bottomborder_anim);

    connect(animgroup, &QParallelAnimationGroup::finished, [this] () {
        _isChannelWidgetShowing = false;
        this->animFinish();
        QRect pauseGeo = ui->pauseWidget->geometry();
        pauseGeo.setHeight(this->geometry().height());
        ui->pauseWidget->setGeometry(pauseGeo);
        this->setMaximumHeight(this->height());
    });
    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWidget::animShowChannelWidget() {
    if (isChannelWidgetShowing() || isAnimationStarted()) return;
    this->animStart();
    auto _maxhei = ui->controlWidget->height() + ui->channelWidget->height();
    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup(this);
    if (_maxhei > this->height()) {
        this->setMaximumHeight(_maxhei);

        QPropertyAnimation *main_anim = new QPropertyAnimation(this, "geometry");
        main_anim->setDuration(400);
        main_anim->setStartValue(this->geometry());
        QRect endval2 = this->geometry();
        endval2.setHeight(endval2.height() + ui->channelWidget->geometry().height());
        main_anim->setEndValue(endval2);
        main_anim->setEasingCurve(QEasingCurve::OutCubic);

        animgroup->addAnimation(main_anim);
    }

    QPropertyAnimation *control_anim = new QPropertyAnimation(ui->controlWidget, "pos");
    control_anim->setDuration(400);
    control_anim->setStartValue(ui->controlWidget->pos());
    QPoint endpos = ui->controlWidget->pos();
    endpos.setY(ui->channelWidget->height());
    control_anim->setEndValue(endpos);
    control_anim->setEasingCurve(QEasingCurve::OutCubic);

    animgroup->addAnimation(control_anim);

    QPropertyAnimation *topborder_anim = new QPropertyAnimation(this->topBorder, "pos");
    topborder_anim->setDuration(400);
    QPoint topborder_pos = topBorder->pos();
    topborder_anim->setStartValue(topborder_pos);
    topborder_pos.setY(ui->channelWidget->height() - topBorder->height());
    topborder_anim->setEndValue(topborder_pos);
    topborder_anim->setEasingCurve(QEasingCurve::OutCubic);
    animgroup->addAnimation(topborder_anim);

    QPropertyAnimation *bottomborder_anim = new QPropertyAnimation(this->bottomBorder, "pos");
    bottomborder_anim->setDuration(400);
    QPoint bottomborder_pos = bottomBorder->pos();
    bottomborder_anim->setStartValue(bottomborder_pos);
    bottomborder_pos.setY(ui->channelWidget->height() + ui->controlWidget->height());
    bottomborder_anim->setEndValue(bottomborder_pos);
    bottomborder_anim->setEasingCurve(QEasingCurve::OutCubic);
    animgroup->addAnimation(bottomborder_anim);

    connect(animgroup, &QParallelAnimationGroup::finished, [this] () {
        _isChannelWidgetShowing = true;
        this->animFinish();
        ui->pauseWidget->setGeometry(0, 0, ui->pauseWidget->geometry().width(), this->geometry().height());
        this->setMinimumHeight(this->height());
    });
    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

bool MainWidget::isChannelWidgetShowing() const {
    return _isChannelWidgetShowing;
}

bool MainWidget::isAnimationStarted() const {
    return _isAnimStarted;
}

bool MainWidget::isLyricWidgetShowing() const {
    return _isLyricWidgetShowing;
}

void MainWidget::animStart() {
    _isAnimStarted = true;
    emit animationStart();
}

void MainWidget::animFinish() {
    _isAnimStarted = false;
    emit animationFinish();
}

void MainWidget::animHideLyricWidget() {
    if (!isLyricWidgetShowing() || isAnimationStarted()) return;
    this->animStart();
    this->setMinimumHeight(ui->controlWidget->pos().y() + ui->controlWidget->height());
    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup(this);
    QPoint lyric_cur = ui->lyricWidget->pos();
    QRect self_cur = this->geometry();

    QPropertyAnimation *lyric_anim = new QPropertyAnimation(ui->lyricWidget, "pos");
    lyric_anim->setDuration(400);
    lyric_anim->setStartValue(lyric_cur);
    lyric_cur.setY(ui->controlWidget->pos().y()
                   + ui->controlWidget->height()
                   - ui->lyricWidget->height());
    lyric_anim->setEndValue(lyric_cur);
    lyric_anim->setEasingCurve(QEasingCurve::OutCubic);
    animgroup->addAnimation(lyric_anim);

    QPropertyAnimation *self_anim = new QPropertyAnimation(this, "geometry");
    self_anim->setDuration(400);
    self_anim->setStartValue(self_cur);
    self_cur.setHeight(ui->controlWidget->pos().y() + ui->controlWidget->height());
    self_anim->setEndValue(self_cur);
    self_anim->setEasingCurve(QEasingCurve::OutCubic);
    animgroup->addAnimation(self_anim);

    connect(animgroup, &QAnimationGroup::finished, [=] () {
        _isLyricWidgetShowing = false;
        animFinish();
        ui->pauseWidget->setGeometry(0, 0, ui->pauseWidget->geometry().width(), this->geometry().height());
        this->setMaximumHeight(this->height());
        ui->lyricWidget->setShowing(false);
        ui->lyricWidget->move(0, ui->controlWidget->height()
                              - ui->lyricWidget->height());
    });

    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWidget::animShowLyricWidget() {
    if (isLyricWidgetShowing() || isAnimationStarted()) return;
    this->animStart();
    this->setMaximumHeight(this->height() + ui->lyricWidget->height());
    QParallelAnimationGroup *animgroup = new QParallelAnimationGroup(this);
    QPoint lyric_cur = ui->lyricWidget->pos();
    QRect self_cur = this->geometry();

    QPropertyAnimation *lyric_anim = new QPropertyAnimation(ui->lyricWidget, "pos");
    lyric_anim->setDuration(400);
    lyric_anim->setStartValue(lyric_cur);
    lyric_cur.setY(ui->controlWidget->height());
    lyric_anim->setEndValue(lyric_cur);
    lyric_anim->setEasingCurve(QEasingCurve::OutCubic);
    animgroup->addAnimation(lyric_anim);

    QPropertyAnimation *self_anim = new QPropertyAnimation(this, "geometry");
    self_anim->setDuration(400);
    self_anim->setStartValue(self_cur);
    self_cur.setHeight(ui->controlWidget->height() + ui->lyricWidget->height());
    self_anim->setEndValue(self_cur);
    self_anim->setEasingCurve(QEasingCurve::OutCubic);
    animgroup->addAnimation(self_anim);

    connect(animgroup, &QAnimationGroup::finished, [=] () {
        _isLyricWidgetShowing = true;
        animFinish();
        ui->pauseWidget->setGeometry(0, 0, ui->pauseWidget->geometry().width(), this->geometry().height());
        this->setMinimumHeight(this->height());
        ui->lyricWidget->setShowing(true);
    });

    animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}
