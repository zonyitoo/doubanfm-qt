#include "pausemask.h"
#include "ui_pausemask.h"
#include <QMouseEvent>

PauseMask::PauseMask(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PauseMask)
{
    ui->setupUi(this);
}

PauseMask::~PauseMask()
{
    delete ui;
}

void PauseMask::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setVisible(false);
    }
    emit clicked();
}
