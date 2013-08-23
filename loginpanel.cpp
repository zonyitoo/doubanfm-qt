#include "loginpanel.h"
#include "ui_loginpanel.h"
#include <QPropertyAnimation>
#include <QDebug>

LoginPanel::LoginPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPanel)
{
    ui->setupUi(this);
    maxheight = 173;
    ui->errmsg->setVisible(false);
    ui->name->setVisible(false);
    ui->password->setVisible(false);
    ui->submit->setVisible(false);
    this->setVisible(false);
    doubanfm = DoubanFM::getInstance();

    connect(doubanfm, &DoubanFM::loginSucceed, [this] (std::shared_ptr<DoubanUser> user) {
        this->animHide();
        qDebug() << "Login succeed:" << user->user_name << user->email;
    });
    connect(doubanfm, &DoubanFM::loginFailed, [this] (const QString& errmsg) {
        ui->errmsg->setText(QString("<font color=red>") + errmsg + QString("</font>"));
        qDebug() << "Login failed:" << errmsg;
    });
}

LoginPanel::~LoginPanel()
{
    delete ui;
}

void LoginPanel::animShow() {
    QPropertyAnimation *anim = new QPropertyAnimation(this, "geometry");
    anim->setDuration(300);
    anim->setStartValue(this->geometry());
    QRect endval = this->geometry();
    endval.setHeight(maxheight);
    anim->setEndValue(endval);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &QPropertyAnimation::finished, [this] () {
        ui->errmsg->setVisible(true);
        ui->name->setVisible(true);
        ui->password->setVisible(true);
        ui->submit->setVisible(true);
    });
    ui->bg->setVisible(true);
    this->setVisible(true);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void LoginPanel::animHide() {
    QPropertyAnimation *anim = new QPropertyAnimation(this, "geometry");
    anim->setDuration(300);
    anim->setStartValue(this->geometry());
    QRect endval = this->geometry();
    endval.setHeight(0);
    anim->setEndValue(endval);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    ui->errmsg->setVisible(false);
    ui->name->setVisible(false);
    ui->password->setVisible(false);
    ui->submit->setVisible(false);
    connect(anim, &QPropertyAnimation::finished, [this] () {
        this->setVisible(false);
        ui->bg->setVisible(false);
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void LoginPanel::on_submit_clicked()
{
    QString name = ui->name->text().trimmed();
    QString password = ui->password->text().trimmed();
    doubanfm->userLogin(name, password);
}
