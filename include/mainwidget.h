#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "channelwidget.h"
#include "controlpanel.h"
#include "maskpausewidget.h"
#include "userloginwidget.h"
#include <QNetworkAccessManager>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    
private:
    Ui::MainWidget *ui;

    ChannelWidget *_channelWidget;
    ControlPanel *_controlPanel;
    MaskPauseWidget *_mpw;
    UserLoginWidget *_userLoginWidget;

    QNetworkAccessManager *_networkmgr;

    void mousePressEvent(QMouseEvent *event);

private slots:
    void onRecvImage(QNetworkReply *reply);
    void onImageChanged(const QString& imgurl);
    void onMaskPressed();
};

#endif // MAINWIDGET_H
