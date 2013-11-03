#ifndef MainWidget_H
#define MainWidget_H

#include <QWidget>
#include <QMouseEvent>
#include "channelwidget.h"
#include "controlpanel.h"
#include "loginpanel.h"
#include "pausemask.h"
#include "lyricwidget.h"
#include <QShortcut>
#include <QLabel>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

    void mousePressEvent(QMouseEvent *);
    bool isChannelWidgetShowing() const;
    bool isAnimationStarted() const;
    bool isLyricWidgetShowing() const;

signals:
    void animationStart();
    void animationFinish();
    
private:
    Ui::MainWidget *ui;
    QPoint dpos;
    QShortcut *exitShortcut;
    QShortcut *pauseShortcut;
    QShortcut *nextShortcut;
    QShortcut *deleteShortcut;
    QShortcut *likeShortcut;

    bool _isChannelWidgetShowing;
    bool _isAnimStarted;
    bool _isLyricWidgetShowing;

    QLabel *topBorder;
    QLabel *bottomBorder;

    void animStart();
    void animFinish();

public slots:
    void animHideChannelWidget();
    void animShowChannelWidget();

    void animHideLyricWidget();
    void animShowLyricWidget();
};

#endif // MainWidget_H
