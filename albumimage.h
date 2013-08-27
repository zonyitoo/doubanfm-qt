#ifndef ALBUMIMAGE_H
#define ALBUMIMAGE_H

#include <QLabel>
#include <QMouseEvent>

class AlbumImage : public QLabel
{
    Q_OBJECT
public:
    explicit AlbumImage(QWidget *parent = 0);
    
    void mousePressEvent(QMouseEvent *ev);
signals:
    void clicked();

public slots:
    void setAlbumImage(const QImage& src);
    
};

#endif // ALBUMIMAGE_H
