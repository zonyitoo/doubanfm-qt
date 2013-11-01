#ifndef ALBUMWIDGET_H
#define ALBUMWIDGET_H

#include <QWidget>

namespace Ui {
class AlbumWidget;
}

class AlbumWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumWidget(QWidget *parent = 0);
    ~AlbumWidget();

    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

signals:
    void clicked();

public slots:
    void setAlbumImage(const QImage& src);

private:
    Ui::AlbumWidget *ui;
};

#endif // ALBUMWIDGET_H
