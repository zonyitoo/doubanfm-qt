#include "userloginwidget.h"
#include "ui_userloginwidget.h"

UserLoginWidget::UserLoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserLoginWidget)
{
    ui->setupUi(this);

    _douban = Douban::getInstance();
    //ui->loginErrMsg->setVisible(false);

    connect(_douban, SIGNAL(loginSucceed(DoubanUser)),
            this, SLOT(recvLoginSucceed(DoubanUser)));
}

UserLoginWidget::~UserLoginWidget()
{
    delete ui;
}

void UserLoginWidget::setInfo(const QString &username, const QString &password) {
    ui->userNameEdit->setText(username);
    ui->passwordEdit->setText(password);
}

void UserLoginWidget::on_submitButton_clicked() {
    QString em = ui->userNameEdit->text().trimmed();
    QString pw = ui->passwordEdit->text().trimmed();

    _douban->doLogin(em, pw);

    ui->userNameEdit->setEnabled(false);
    ui->passwordEdit->setEnabled(false);
    ui->submitButton->setEnabled(false);
    //ui->loginErrMsg->setVisible(true);
    //ui->loginErrMsg->setText(QString("<font color=green>登录中</font>"));
}

void UserLoginWidget::recvLoginSucceed(DoubanUser user) {
    user.password = ui->passwordEdit->text().trimmed();

    ui->userNameEdit->setEnabled(true);
    ui->passwordEdit->setEnabled(true);
    ui->submitButton->setEnabled(true);

    emit loginSucceed(user);
    //ui->loginErrMsg->setVisible(false);
}

void UserLoginWidget::recvLoginFailed(const QString &errmsg) {
    ui->userNameEdit->setEnabled(true);
    ui->passwordEdit->setEnabled(true);
    ui->submitButton->setEnabled(true);
    //ui->submitButton->setText(errmsg);
    ui->userNameEdit->setText("");
    ui->passwordEdit->setText("");
    if (errmsg == "invalidate_email")
        ui->userNameEdit->setPlaceholderText(errmsg);
    else
        ui->passwordEdit->setPlaceholderText(errmsg);

    //ui->loginErrMsg->setText(QString("<font color='red'>")
    //                         + errmsg + QString("</font>"));
}
