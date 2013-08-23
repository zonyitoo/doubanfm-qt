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
    
private slots:
    void on_toolButton_clicked();

    void on_volumeButton_clicked();

private:
    Ui::TitleWidget *ui;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QPoint dpos;
    QWidget *parent;
};

#endif // TITLEWIDGET_H
