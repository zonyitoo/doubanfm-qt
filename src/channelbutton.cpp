#include "channelbutton.h"

ChannelButton::ChannelButton(QWidget *parent) :
    QPushButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
}

void ChannelButton::on_clicked() {
    emit clicked(this);
}
