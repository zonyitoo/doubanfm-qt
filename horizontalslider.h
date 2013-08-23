#ifndef HORIZONTALSLIDER_H
#define HORIZONTALSLIDER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QObjectList>

class HorizontalSlider : public QWidget
{
    Q_OBJECT
public:
    explicit HorizontalSlider(QWidget *parent = 0);
    ~HorizontalSlider();

    void setChildren(const QObjectList& list);

    void scrollToIndex(int index);
    int numberOfChildren();

    int currentIndex();

private:
    QHBoxLayout *hbox;
    QWidget *container;
    QList<int> widths;
    int curIndex;

    void refresh();
    
signals:
    void scrollFinished();
};

#endif // HORIZONTALSLIDER_H
