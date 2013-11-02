#ifndef LOGINPANEL_H
#define LOGINPANEL_H

#include <QWidget>
#include "libs/doubanfm.h"
#include "libs/douban_types.h"

namespace Ui {
class LoginPanel;
}

class LoginPanel : public QWidget
{
    Q_OBJECT
    
public:
    explicit LoginPanel(QWidget *parent = 0);
    ~LoginPanel();

    void animShow();
    void animHide();

    bool isShowing();
    
private slots:
    void on_submit_clicked();

private:
    Ui::LoginPanel *ui;
    int maxheight;
    bool _isShowing;
    DoubanFM *doubanfm;
};

#endif // LOGINPANEL_H
