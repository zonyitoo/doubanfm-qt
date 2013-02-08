#ifndef MASKPAUSEWIDGET_H
#define MASKPAUSEWIDGET_H

#include <QWidget>

class MaskPauseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaskPauseWidget(QWidget *parent = 0);
    
signals:
    void mousePressed();

public slots:

private:
    void mousePressEvent(QMouseEvent *event);
    
};

#endif // MASKPAUSEWIDGET_H
