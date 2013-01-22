#ifndef DOUBANLOGINDIALOG_H
#define DOUBANLOGINDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "douban_types.h"
#include <QtGui>
#include <QRegExp>

namespace Ui {
class DoubanLoginDialog;
}

class DoubanLoginDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DoubanLoginDialog(QWidget *parent = 0);
    ~DoubanLoginDialog();
    void setNameAndPassword(const QString &email, const QString &password);

signals:
    void loginSucceed(DoubanUser user);
    
private:
    Ui::DoubanLoginDialog *ui;
    QNetworkAccessManager *networkmgr;
    QRegExp emailRegExp;

    void doLogin(const QString &email, const QString &password);

private slots:
    void onReceivedLogin(QNetworkReply *reply);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // DOUBANLOGINDIALOG_H
