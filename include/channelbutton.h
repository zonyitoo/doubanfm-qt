#ifndef CHANNELBUTTON_H
#define CHANNELBUTTON_H

#include <QPushButton>

class ChannelButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ChannelButton(QWidget *parent = 0);
    
signals:
    void clicked(const ChannelButton *button);

private slots:
    void on_clicked();
};

#endif // CHANNELBUTTON_H
