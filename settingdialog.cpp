#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QButtonGroup>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtXml/QDomDocument>
#include "libs/doubanplayer.h"

#ifdef MSVC2012
#include "STRINGS_GBK.H"
#else
#include "STRINGS_UTF8.H"
#endif


SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog),
    doubanfm(DoubanFM::getInstance()),
    kbpsGroup(new QButtonGroup(this)),
    userIconGetter(new QNetworkAccessManager(this)),
    userInfoGetter(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    kbpsGroup->addButton(ui->kbps64);
    kbpsGroup->addButton(ui->kbps128);
    kbpsGroup->addButton(ui->kbps192);
    connect(kbpsGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(kbps_radio_button_clicked(QAbstractButton*)));



    connect(userIconGetter, &QNetworkAccessManager::finished, [=] (QNetworkReply *reply) {
        if (QNetworkReply::NoError != reply->error()) {
            qDebug() << "Err: User icon image receive error";
            reply->deleteLater();
            return;
        }
        const QByteArray data(reply->readAll());
        if (!data.size())
            qDebug() << Q_FUNC_INFO << "received pixmap looks like nothing";
        QImage image = QImage::fromData(data);
        QPixmap pixmap = QPixmap::fromImage(image);
        this->setUserIcon(pixmap);
        reply->deleteLater();
    });

    connect(userInfoGetter, &QNetworkAccessManager::finished, [=] (QNetworkReply *reply) {
        if (QNetworkReply::NoError != reply->error()) {
            qDebug() << "Err: User info receive error";
            reply->deleteLater();
            return;
        }
        const QByteArray data(reply->readAll());
        QDomDocument info;
        if (!info.setContent(data)) {
            qDebug() << "Err: User info receive error";
            reply->deleteLater();
            return;
        }
        QDomNodeList links = info.elementsByTagName("link");
        for (int i = 0; i < links.size(); ++ i) {
            QDomElement elem = links.at(i).toElement();
            if (elem.attribute("rel") == "icon") {
                userIconGetter->get(QNetworkRequest(QUrl(elem.attribute("href"))));
                break;
            }
        }
        reply->deleteLater();
    });

    connect(&doubanfm, &DoubanFM::loginSucceed, [=] (const DoubanUser &user) {
        userInfoGetter->get(QNetworkRequest(QUrl("http://api.douban.com/people/" + user.user_id)));
        ui->loginButton->setText(tr("登出"));
        ui->usernameLabel->setText(user.user_name);
        ui->loginButton->setEnabled(true);
    });

    connect(&doubanfm, &DoubanFM::loginFailed, [=] (const QString& msg) {
        ui->email->setEnabled(true);
        ui->password->setEnabled(true);
        ui->loginButton->setText(msg);
        QTimer::singleShot(3000, this, SLOT(timer_event()));
    });

    QSettings settings("QDoubanFM", "QDoubanFM");
    settings.beginGroup("General");
    qint32 _kbps = settings.value("kbps", 64).toInt();
    settings.endGroup();
    switch (_kbps) {
    case 64:
        ui->kbps64->setChecked(true);
        break;
    case 128:
        ui->kbps128->setChecked(true);
        break;
    case 192:
        ui->kbps192->setChecked(true);
        break;
    }
}

SettingDialog::~SettingDialog()
{
    delete kbpsGroup;
    delete ui;
}

void SettingDialog::setUserIcon(const QPixmap &pixmap) {
    ui->userIcon->setPixmap(pixmap);
}

void SettingDialog::on_loginButton_clicked()
{
    if (!doubanfm.hasLogin()) {
        QString email = ui->email->text();
        QString pwd = ui->password->text();
        if (email.size() == 0 || pwd.size() == 0) return;
        doubanfm.userLogin(email, pwd);
        ui->email->setEnabled(false);
        ui->password->setEnabled(false);
        ui->loginButton->setEnabled(false);
        ui->loginButton->setText(tr("登录中..."));
    }
    else {
        doubanfm.userLogout();
        ui->email->setText("");
        ui->password->setText("");
        ui->email->setEnabled(true);
        ui->password->setEnabled(true);
        ui->loginButton->setText(tr("登录"));
        ui->userIcon->setPixmap(QPixmap(":/img/user_man_circle.png"));
        ui->usernameLabel->setText(tr(STRINGS_NOTLOGIN));
    }
}

void SettingDialog::timer_event() {
    ui->email->setEnabled(true);
    ui->password->setEnabled(true);
    ui->loginButton->setEnabled(true);
    ui->loginButton->setText(tr("登录"));
}

void SettingDialog::kbps_radio_button_clicked(QAbstractButton *button) {
    auto& player = DoubanPlayer::getInstance();
    if (button == ui->kbps64) {
        player.setKbps(64);
    }
    else if (button == ui->kbps128) {
        player.setKbps(128);
    }
    else if (button == ui->kbps192) {
        player.setKbps(192);
    }
}

void SettingDialog::show(){
    auto user = doubanfm.getUser();
    if (doubanfm.hasLogin()) {

        ui->email->setEnabled(false);
        ui->password->setEnabled(false);
        ui->loginButton->setText(tr("登出"));
        ui->email->setText(user->email);
        ui->password->setText(/*user->password*/"****************");
        ui->usernameLabel->setText(user->user_name);
        userInfoGetter->get(QNetworkRequest(QUrl("http://api.douban.com/people/" + user->user_id)));
    }else{
         if(user)
             ui->email->setText(user->email);

        ui->email->setEnabled(true);
        ui->password->setEnabled(true);
        ui->loginButton->setEnabled(true);
        ui->loginButton->setText(tr("登录"));
    }
    QWidget::show();
}
