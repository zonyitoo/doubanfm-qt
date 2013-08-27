#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "channelwidget.h"
#include "controlpanel.h"
#include "loginpanel.h"
#include "pausemask.h"
#include <QShortcut>

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
    LoginPanel *loginPanel();
    PauseMask *pauseMask();

    void mousePressEvent(QMouseEvent *);
    bool isChannelWidgetShowing();
    
private:
    Ui::mainwidget *ui;
    QPoint dpos;
    QShortcut *exitShortcut;
    QShortcut *pauseShortcut;

    bool _isChannelWidgetShowing;

public slots:
    void animHideChannelWidget();
    void animShowChannelWidget();
};

#endif // MAINWIDGET_H
