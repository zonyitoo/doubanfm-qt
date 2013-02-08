#ifndef VOLUMEWIDGET_H
#define VOLUMEWIDGET_H

#include <QWidget>

namespace Ui {
class VolumeWidget;
}

class VolumeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit VolumeWidget(QWidget *parent = 0);
    ~VolumeWidget();

    void setVolume(qreal newvol);
    void animShow();
    void animHide();
    bool isShowing();

signals:
    void volumeChanged(qreal);
    
private slots:
    void on_volumeSlider_valueChanged(int value);

private:
    Ui::VolumeWidget *ui;

    void leaveEvent(QEvent *event);

    bool _isShowing;
};

#endif // VOLUMEWIDGET_H
