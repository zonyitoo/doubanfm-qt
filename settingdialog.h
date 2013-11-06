#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include "libs/doubanfm.h"
#include <QNetworkAccessManager>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

private slots:
    void setUserIcon(const QPixmap& pixmap);

    void on_loginButton_clicked();

    void timer_event();
    void kbps_radio_button_clicked(QAbstractButton *button);

private:
    Ui::SettingDialog *ui;

    DoubanFM& doubanfm;
    QButtonGroup *kbpsGroup;

    QNetworkAccessManager *userIconGetter;
    QNetworkAccessManager *userInfoGetter;
};

#endif // SETTINGDIALOG_H
