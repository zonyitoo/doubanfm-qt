#include "albumwidget.h"
#include "ui_albumwidget.h"

AlbumWidget::AlbumWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AlbumWidget)
{
    ui->setupUi(this);

    connect(ui->albimImage, &AlbumImage::clicked, [this] () {
        emit this->clicked();
    });

    ui->label->raise();
    ui->label->hide();
}

AlbumWidget::~AlbumWidget()
{
    delete ui;
}

void AlbumWidget::setAlbumImage(const QImage &src) {
    ui->albimImage->setAlbumImage(src);
}

void AlbumWidget::enterEvent(QEvent *) {
    ui->label->show();
}

void AlbumWidget::leaveEvent(QEvent *) {
    ui->label->hide();
}
