#ifndef VOLUMETIMEPANEL_H
#define VOLUMETIMEPANEL_H

#include <QWidget>

namespace Ui {
class VolumeTimePanel;
}

class VolumeTimePanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit VolumeTimePanel(QWidget *parent = 0);
    ~VolumeTimePanel();

    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

signals:
    void volumeChanged(int);

public slots:
    void setTick(qint64 tick);
    
private:
    Ui::VolumeTimePanel *ui;
};

#endif // VOLUMETIMEPANEL_H
