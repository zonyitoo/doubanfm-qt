#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class TitleWidget;
}

class TitleWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TitleWidget(QWidget *parent = 0);
    ~TitleWidget();

    void animShowVolume();
    void animHideVolume();

signals:
    void volumeChanged(int);
    
private slots:
    void on_volumeButton_clicked();

    void on_userLogin_clicked();

private:
    Ui::TitleWidget *ui;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QPoint dpos;
    QWidget *parent;

    bool _isVolumeShowing;
    bool _isLoginPanelOpen;
};

#endif // TITLEWIDGET_H
