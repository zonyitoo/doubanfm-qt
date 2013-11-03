#include "pausemask.h"
#include "ui_pausemask.h"
#include <QMouseEvent>
#include "libs/doubanplayer.h"

PauseMask::PauseMask(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PauseMask)
{
    ui->setupUi(this);
    connect(DoubanPlayer::getInstance(), &DoubanPlayer::playing, [=] () {
        this->setVisible(false);
    });
}

PauseMask::~PauseMask()
{
    delete ui;
}

void PauseMask::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setVisible(false);
        emit clicked();
    }
}
