#include "channelwidget.h"
#include "ui_channelwidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QPropertyAnimation>

ChannelWidget::ChannelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelWidget)
{
    ui->setupUi(this);

    _douban = Douban::getInstance();
    connect(_douban, SIGNAL(receivedChannels(QList<DoubanChannel>)),
            this, SLOT(onRecvChannels(QList<DoubanChannel>)));

    _channelContainer = new QWidget(ui->channel);
    _channelContainer->setMaximumHeight(50);
    _channelContainer->setMinimumHeight(50);
    _channelContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QHBoxLayout *l = new QHBoxLayout(_channelContainer);
    l->setSizeConstraint(QLayout::SetFixedSize);
    l->setMargin(0);
    l->setSpacing(0);
    _channelContainer->setLayout(l);

    ui->leftArrow->setPixmap(QPixmap(":/icons/channel_left.png"));
    ui->rightArrow->setPixmap(QPixmap(":/icons/channel_right.png"));

    curValueIndex = 0;
    curChannelId = 1;

    refreshChannels();
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

int ChannelWidget::getInvisibleHeight() {
    return ui->channel->geometry().height();
}

void ChannelWidget::refreshChannels() {
    _douban->getChannels();
}

void ChannelWidget::setChannelId(qint32 channel_id) {
    curChannelId = channel_id;

    for (int i = 0; i < _channels.size(); ++ i) {
        if (curChannelId == _channels[i].channel_id) {
            curValueIndex = i;
            _channelContainer->move(116 - 116 * curValueIndex, 0);
            _channelLabels[curValueIndex]->setText(QString("<font color='white'>")
                                                   + _channels[curValueIndex].name
                                                   + QString("</font>"));
            ui->currentChannel->setText(QString("<font color='white'>")
                                        + _channels[curValueIndex].name
                                        + QString("</font>"));
            break;
        }
    }
}

void ChannelWidget::onRecvChannels(const QList<DoubanChannel>& channels) {
    qDebug() << Q_FUNC_INFO;
    _channels = channels;
    for (int i = 0; i < channels.size(); ++ i) {
        DoubanChannel channel = channels[i];
        QLabel *lb = new QLabel(_channelContainer);
        lb->setFont(QFont("sans serif", 15));
        lb->setText(QString("<font color='grey'>") + channel.name + QString("</font>"));
        lb->setMinimumSize(116, 50);
        lb->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        _channelLabels.push_back(lb);
        _channelContainer->layout()->addWidget(lb);

        if (curChannelId == channel.channel_id) curValueIndex = i;
    }
    _channelContainer->move(116 - 116 * curValueIndex, 0);
    _channelLabels[curValueIndex]->setText(QString("<font color='white'>")
                                           + _channels[curValueIndex].name
                                           + QString("</font>"));
    ui->currentChannel->setText(QString("<font color='white'>")
                                + _channels[curValueIndex].name
                                + QString("</font>"));
}

void ChannelWidget::onAnimFinished() {
    _channelLabels[curValueIndex]->setText(QString("<font color='white'>")
                                           + _channels[curValueIndex].name
                                           + QString("</font>"));
    //ui->currentChannel->setText(QString("<font color='white'>")
    //                            + _channels[curValueIndex].name
    //                            + QString("</font>"));
    emit channelChanged(_channels[curValueIndex]);
    emit channelChanged(_channels[curValueIndex].channel_id);
}

void ChannelWidget::mousePressEvent(QMouseEvent *event) {
    if (event->x() < 116) {
        if (curValueIndex == _channels.size() - 1) return;

        _channelLabels[curValueIndex]->setText(QString("<font color='grey'>")
                                               + _channels[curValueIndex].name
                                               + QString("</font>"));

        QPropertyAnimation *anim = new QPropertyAnimation(_channelContainer, "geometry");
        anim->setDuration(300);
        anim->setStartValue(_channelContainer->geometry());
        QRect end(116 - 116 * (++ curValueIndex),
                  _channelContainer->geometry().y(),
                  _channelContainer->geometry().width(),
                  _channelContainer->geometry().height());
        anim->setEndValue(end);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, SIGNAL(finished()), this, SLOT(onAnimFinished()));
        anim->start();
    }
    else if (event->x() > 232) {
        if (this->curValueIndex == 0) return;

        _channelLabels[curValueIndex]->setText(QString("<font color='grey'>")
                                               + _channels[curValueIndex].name
                                               + QString("</font>"));

        QPropertyAnimation *anim = new QPropertyAnimation(_channelContainer, "geometry");
        anim->setDuration(300);
        anim->setStartValue(_channelContainer->geometry());
        QRect end(116 - 116 * (-- curValueIndex),
                  _channelContainer->geometry().y(),
                  _channelContainer->geometry().width(),
                  _channelContainer->geometry().height());
        anim->setEndValue(end);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, SIGNAL(finished()), this, SLOT(onAnimFinished()));
        anim->start();
    }
}

void ChannelWidget::enterEvent(QEvent *event) {
    ui->currentChannel->setText(QString("<font color='white'>")
                                + QString(tr("Choose Channel"))
                                + QString("</font>"));

    QPropertyAnimation *anim = new QPropertyAnimation(this, "geometry");
    anim->setStartValue(this->geometry());
    QRect endVal(this->geometry().x(),
                 0,
                 this->geometry().width(),
                 this->geometry().height());
    anim->setEndValue(endVal);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start();
}

void ChannelWidget::leaveEvent(QEvent *event) {
    ui->currentChannel->setText(QString("<font color='white'>")
                                + _channels[curValueIndex].name
                                + QString("</font>"));

    QPropertyAnimation *anim = new QPropertyAnimation(this, "geometry");
    anim->setStartValue(this->geometry());
    QRect endVal(this->geometry().x(),
                 -this->getInvisibleHeight(),
                 this->geometry().width(),
                 this->geometry().height());
    anim->setEndValue(endVal);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start();
}
