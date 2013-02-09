#include "maskpausewidget.h"
#include <QDebug>
#include <QtGui>

MaskPauseWidget::MaskPauseWidget(QWidget *parent) :
    QWidget(parent)
{
    this->setMaximumSize(350, 350);
    this->setMinimumSize(350, 350);
    this->setObjectName("MaskPauseWidget");
    this->setStyleSheet("QLabel#MaskBottomLabel {background-color: rgba(0,0,0,200);}");

    QLabel *la = new QLabel(this);
    la->setObjectName("MaskBottomLabel");
    la->setMaximumSize(350, 350);
    la->setMinimumSize(350, 350);

    QLabel *pause = new QLabel(this);
    pause->setMaximumSize(128, 128);
    pause->setMinimumSize(128, 128);
    pause->move(111, 111);
    QPixmap pausePixmap;
    pausePixmap.load(":/icons/pause.png");
    pause->setPixmap(pausePixmap.scaled(128, 128));
}

void MaskPauseWidget::mousePressEvent(QMouseEvent *event) {
    emit mousePressed();
}
