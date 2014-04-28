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

    void setChildren(QList<QWidget*> *list);

    void scrollToIndex(int index);
    int numberOfChildren();

    int currentIndex();
    QWidget* currentObject(){  return (*items)[curIndex];    }
    QWidget* preObject(){
        if(curIndex>0)
            return (*items)[curIndex - 1];
        else
            return nullptr;
    }
    QWidget* nextObject(){
        if(curIndex<items->length()-1)
            return (*items)[curIndex + 1];
        else
            return nullptr;
    }

private:
    QHBoxLayout *hbox;
    QWidget *container;
    QList<QWidget*>* items;
    QList<int> widths;
    int curIndex;

    void refresh();
    
signals:
    void scrollFinished();
};

#endif // HORIZONTALSLIDER_H
