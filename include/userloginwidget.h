#ifndef USERLOGINWIDGET_H
#define USERLOGINWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <douban.h>
#include <douban_types.h>

namespace Ui {
class UserLoginWidget;
}

class UserLoginWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit UserLoginWidget(QWidget *parent = 0);
    ~UserLoginWidget();

    void setInfo(const QString& username, const QString& password);

signals:
    void loginSucceed(DoubanUser user);

public slots:
    void recvLoginSucceed(DoubanUser user);
    void recvLoginFailed(const QString& errmsg);
private slots:
    void on_submitButton_clicked();

private:
    Ui::UserLoginWidget *ui;

    Douban *_douban;
};

#endif // USERLOGINWIDGET_H
