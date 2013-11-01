#ifndef ALBUMIMAGE_H
#define ALBUMIMAGE_H

#include <QLabel>
#include <QMouseEvent>
#include <QImage>

class AlbumImage : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

public:
    explicit AlbumImage(QWidget *parent = 0);
    
    void mousePressEvent(QMouseEvent *ev);
    qreal opacity() const;
signals:
    void clicked();
    void opacityChanged(qreal opacity);

public slots:
    void setAlbumImage(const QImage& src);
    void setOpacity(qreal opacity);

private:
    qreal _opacity;
    QImage before;
    QImage now;
};

#endif // ALBUMIMAGE_H
