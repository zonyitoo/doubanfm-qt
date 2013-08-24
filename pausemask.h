#ifndef PAUSEMASK_H
#define PAUSEMASK_H

#include <QWidget>
#include <QShortcut>

namespace Ui {
class PauseMask;
}

class PauseMask : public QWidget
{
    Q_OBJECT
    
public:
    explicit PauseMask(QWidget *parent = 0);
    ~PauseMask();

    void mousePressEvent(QMouseEvent *event);

signals:
    void clicked();
    
private:
    Ui::PauseMask *ui;
};

#endif // PAUSEMASK_H
