#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "channelwidget.h"
#include "controlpanel.h"
#include "titlewidget.h"
#include "loginpanel.h"

namespace Ui {
class mainwidget;
}

class mainwidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit mainwidget(QWidget *parent = 0);
    ~mainwidget();

    ChannelWidget *channelWidget();
    ControlPanel *controlPanel();
    TitleWidget *titleWidget();
    LoginPanel *loginPanel();
    
private:
    Ui::mainwidget *ui;
    QPoint dpos;
};

#endif // MAINWIDGET_H
