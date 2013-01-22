#include "doubanlogindialog.h"
#include "ui_doubanlogindialog.h"
#include <qjson/parser.h>

static const QString DOUBAN_FM_LOGIN = "http://www.douban.com/j/app/login";

DoubanLoginDialog::DoubanLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoubanLoginDialog)
{
    ui->setupUi(this);
    networkmgr = new QNetworkAccessManager(this);

    connect(networkmgr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onReceivedLogin(QNetworkReply*)));

    emailRegExp.setPatternSyntax(QRegExp::RegExp2);
    emailRegExp.setCaseSensitivity(Qt::CaseSensitive);
    emailRegExp.setPattern("^[a-zA-Z_]+([\\.-]?[a-zA-Z_]+)*@[a-zA-Z_]+([\\.-]?[a-zA-Z_]+)*(\\.[a-zA-Z_]{2,3})+");
}

DoubanLoginDialog::~DoubanLoginDialog()
{
    delete ui;
    delete networkmgr;
}

void DoubanLoginDialog::setNameAndPassword(const QString &email, const QString &password) {
    ui->emailLineEdit->setText(email);
    ui->passwordLineEdit->setText(password);
}

void DoubanLoginDialog::doLogin(const QString &email, const QString &password) {
    QString args = QString("app_name=radio_desktop_win&version=100")
            + QString("&email=") + email
            + QString("&password=") + password;
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QVariant("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(args.length()));
    request.setUrl(QUrl(DOUBAN_FM_LOGIN));
    networkmgr->post(request, args.toAscii());
}

void DoubanLoginDialog::onReceivedLogin(QNetworkReply *reply) {
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QString all = codec->toUnicode(reply->readAll());

    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(all.toAscii(), &ok);

    if (ok) {
        QVariantMap obj = result.toMap();
        if (obj["r"].toInt() != 0) {
            ui->state->setText(QString("<font color=red>")
                      + obj["err"].toString()
                      + QString("</font>"));
            return;
        }
        DoubanUser nuser;
        nuser.user_id = obj["user_id"].toString();
        nuser.expire = obj["expire"].toString();
        nuser.token = obj["token"].toString();
        nuser.user_name = obj["user_name"].toString();
        nuser.email = obj["email"].toString();
        nuser.password = ui->passwordLineEdit->text();

        emit loginSucceed(nuser);

        this->hide();
    }
    else {
        ui->state->setText("网络连接错误");
    }
    reply->deleteLater();
}

void DoubanLoginDialog::on_buttonBox_accepted() {
    if (emailRegExp.exactMatch(ui->emailLineEdit->text().trimmed())
            && !ui->passwordLineEdit->text().trimmed().isEmpty()) {
        doLogin(ui->emailLineEdit->text(), ui->passwordLineEdit->text());
    }
    else {
        ui->state->setText("<font color=red>请输入完整的用户名和密码</font>");
    }
}

void DoubanLoginDialog::on_buttonBox_rejected() {
    this->hide();
}
