#ifndef TRIGGERAREA_H
#define TRIGGERAREA_H

#include <QWidget>

class TriggerArea : public QWidget
{
    Q_OBJECT
public:
    explicit TriggerArea(QWidget *parent = 0);
    
    void enterEvent(QEvent *ev);
    void leaveEvent(QEvent *ev);
signals:
    void enter();
    void leave();
};

#endif // TRIGGERAREA_H
