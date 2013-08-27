#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPoint>
#include <QMouseEvent>

mainwidget::mainwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainwidget)
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