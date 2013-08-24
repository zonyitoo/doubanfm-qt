#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QPoint>
#include <QMouseEvent>

mainwidget::mainwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainwidget)
{
    ui->setupUi(this);
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

TitleWidget *mainwidget::titleWidget() {
    return ui->titleWidget;
}

LoginPanel *mainwidget::loginPanel() {
    return ui->loginWidget;
}

void mainwidget::mousePressEvent(QMouseEvent *) {
    if (this->loginPanel()->isShowing())
        this->loginPanel()->animHide();
}
