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
}

mainwidget::~mainwidget()
{
    delete ui;
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
