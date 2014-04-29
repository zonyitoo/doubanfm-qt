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

    void setChildren(const QList<QWidget *>& list);
    void addChild(QWidget *widget);

    void scrollToIndex(int index);
    int numberOfChildren();

    void clear();

    int currentIndex();
    QWidget *currentObject() const;
    QWidget *preObject() const;
    QWidget *nextObject() const;

private:
    QHBoxLayout *hbox;
    QWidget *container;
    QList<QWidget *> items;
    QList<int> widths;
    int curIndex;

    void refresh();
    
signals:
    void scrollFinished();
};

#endif // HORIZONTALSLIDER_H
