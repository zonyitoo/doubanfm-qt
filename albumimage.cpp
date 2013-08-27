#include "albumimage.h"
#include <QPainter>

AlbumImage::AlbumImage(QWidget *parent) :
    QLabel(parent)
{
}

void AlbumImage::setAlbumImage(const QImage& src) {
    static const QSize picsize(136, 136);

    QImage image = src.scaled(picsize.width(), picsize.height(), Qt::KeepAspectRatioByExpanding)
            .copy(0, 0, picsize.width(), picsize.height());

    /*
    QLinearGradient gardient(QPoint(0, 0), QPoint(0, image.height()));
    gardient.setColorAt(0, Qt::white);
    gardient.setColorAt(0.1, Qt::black);

    QImage mask(picsize, image.format());
    QPainter painter(&mask);
    painter.fillRect(mask.rect(), gardient);
    painter.end();

    QImage reflection = image.mirrored();
    reflection.setAlphaChannel(mask);

    QImage imgdraw(QSize(this->width(), this->height()), QImage::Format_ARGB32_Premultiplied);
    QPainter album_painter(&imgdraw);

    album_painter.drawImage(0, 0, image);
    //album_painter.setOpacity(0.5);
    album_painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    album_painter.drawImage(0, image.height(), reflection);
    album_painter.end();
*/
    QPixmap mirror(image.width(), this->height() - image.height());
    mirror.fill(Qt::transparent);
    QPainter mirrorp(&mirror);
    QLinearGradient linearGrad(QPoint(mirror.width(), 0), QPoint(mirror.width(), mirror.height() / 2));
    linearGrad.setColorAt(1, QColor(255,255,255,0));
    linearGrad.setColorAt(0.8, QColor(255,255,255,20));
    linearGrad.setColorAt(0, QColor(255,255,255,200));
    mirrorp.setBrush(linearGrad);
    mirrorp.fillRect(0, 0, mirror.width(), mirror.height(), QBrush(linearGrad));
    mirrorp.setCompositionMode(QPainter::CompositionMode_SourceIn);
    mirrorp.drawPixmap(0, 0, QPixmap::fromImage(image.copy(0, 2 * image.height() - this->height(), image.width(), this->height() - image.height()).mirrored(false, true)));
    mirrorp.end();

    QImage imgdraw(QSize(this->width(), this->height()), QImage::Format_ARGB32_Premultiplied);
    imgdraw.fill(Qt::transparent);
    QPainter album_painter(&imgdraw);
    album_painter.drawImage(0, 0, image);
    //album_painter.setOpacity(0.8);
    album_painter.drawPixmap(0, image.height(), mirror);
    album_painter.end();
    QPixmap empty(this->size());
    empty.fill(Qt::transparent);
    this->setPixmap(empty);
    this->setPixmap(QPixmap::fromImage(imgdraw));
}

void AlbumImage::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}
