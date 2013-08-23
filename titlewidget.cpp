#include "titlewidget.h"
#include "ui_titlewidget.h"
#include <QMouseEvent>
#include <QPoint>
#include <QDebug>
#include "mainwidget.h"

TitleWidget::TitleWidget(QWidget *parent) :
    QWidget(parent), parent(parent),
    ui(new Ui::TitleWidget)
{
    ui->setupUi(this);
}

TitleWidget::~TitleWidget()
{
    delete ui;
}

void TitleWidget::on_toolButton_clicked()
{
    emit qApp->quit();
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
    static bool isopen = false;
    LoginPanel *loginPanel = dynamic_cast<mainwidget *>(this->parentWidget())->loginPanel();
    if (isopen)
        loginPanel->animHide();
    else
        loginPanel->animShow();
    isopen = !isopen;
}
