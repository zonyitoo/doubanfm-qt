#include "albumimage.h"
#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

AlbumImage::AlbumImage(QWidget *parent) :
    QLabel(parent),
    _opacity(0.0)
{
    before = QImage(":/img/album_init.jpg").scaled(200, 200, Qt::KeepAspectRatioByExpanding);
    this->setPixmap(QPixmap::fromImage(before));
}

void AlbumImage::setAlbumImage(const QImage& src) {
    QPropertyAnimation *anim = new QPropertyAnimation(this, "opacity");
    anim->setDuration(1000);
    anim->setStartValue(this->opacity());
    anim->setEndValue(1.0);
    now = src.scaled(200, 200, Qt::KeepAspectRatioByExpanding).copy(0, 0, 200, 200);

    connect(anim, &QPropertyAnimation::finished, [this] () {
        before = now;
        _opacity = 0.0;
    });
    anim->start(QPropertyAnimation::DeleteWhenStopped);
    //this->setPixmap(QPixmap::fromImage(src.scaled(200, 200, Qt::KeepAspectRatioByExpanding)
    //                                   .copy(0, 0, 200, 200)));
    //QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
    //effect->setOpacity(_opacity);
    /*
    static const QSize picsize(170, 170);

    QImage image = src.scaled(picsize.width(), picsize.height(), Qt::KeepAspectRatioByExpanding)
            .copy(0, 0, picsize.width(), picsize.height());

    // OLD
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

    // NEW
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
    */
}

void AlbumImage::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

qreal AlbumImage::opacity() const {
    return _opacity;
}

void AlbumImage::setOpacity(qreal opacity) {
    _opacity = opacity;
    QPixmap pixmap(this->size());
    QPainter painter(&pixmap);
    painter.setOpacity(_opacity);
    painter.drawImage(0, 0, now);
    painter.setOpacity(1.0 - _opacity);
    painter.drawImage(0, 0, before);
    painter.end();
    this->setPixmap(pixmap);
}


