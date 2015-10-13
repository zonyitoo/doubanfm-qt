#include "channelwidget.h"
#include "ui_channelwidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include "mainwidget.h"
#include "libs/doubanplayer.h"

#define HIGHTLIGHT_STYLE  "font-size:20px;font-style:bold;color:white;"
#define DARK_STYLE  "font-size:15px;font-style:normal; color:grey"
ChannelWidget::ChannelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelWidget),
    doubanfm(DoubanFM::getInstance())
{
    ui->setupUi(this);

    ui->nextButton->raise();
    ui->prevButton->raise();

    connect(&doubanfm, SIGNAL(receivedChannels(QList<DoubanChannel>)),
            this, SLOT(setChannels(QList<DoubanChannel>)));
    doubanfm.getChannels();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=] () {
        qDebug() << "Switch to channel" << channels[ui->slider->currentIndex()].name;
        emit channelChanged(channels[ui->slider->currentIndex()].channel_id);
    });
    connect(ui->slider, &HorizontalSlider::scrollFinished, [this] () {
        channel = channels[ui->slider->currentIndex()].channel_id;
        if (timer && timer->isActive()) 
            timer->stop();
        timer->setSingleShot(true);
        timer->start(2000);
    });

    QSettings settings("QDoubanFM", "QDoubanFM");
    settings.beginGroup("General");
    channel = settings.value("channel", 1).toInt();
    if (!doubanfm.hasLogin() && channel == -3) channel = 1;
    settings.endGroup();

    connect(&doubanfm, &DoubanFM::logoffSucceed, [this] () {
        if (ui->slider->currentIndex() == 0){
            ui->slider->currentObject()->setStyleSheet(DARK_STYLE);
            ui->slider->scrollToIndex(1);
            ui->slider->currentObject()->setStyleSheet(HIGHTLIGHT_STYLE);
        } else {
            doubanfm.getNewPlayList(channel);
        }
        qDebug() << "Logoff successfully. Refreshing playlist";
    });

    connect(this, SIGNAL(channelChanged(qint32)),
            &DoubanPlayer::getInstance(), SLOT(setChannel(qint32)));
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::on_prevButton_clicked()
{
    if (ui->slider->currentIndex() == 0) return;
    if (!doubanfm.hasLogin() && ui->slider->currentIndex() == 1) return;
    ui->slider->scrollToIndex(ui->slider->currentIndex() - 1);

    ui->slider->nextObject()->setStyleSheet(DARK_STYLE);
    ui->slider->currentObject()->setStyleSheet(HIGHTLIGHT_STYLE);

}

void ChannelWidget::on_nextButton_clicked()
{
    if (ui->slider->currentIndex() == ui->slider->numberOfChildren() - 1) return;
    ui->slider->scrollToIndex(ui->slider->currentIndex() + 1);

    ui->slider->preObject()->setStyleSheet(DARK_STYLE);
    ui->slider->currentObject()->setStyleSheet(HIGHTLIGHT_STYLE);

}

void ChannelWidget::setChannels(const QList<DoubanChannel>& channels) {
    this->channels = channels;
    int curindex = 0;
    QList<QWidget *> labels;
    for (int i = 0; i < channels.size(); ++ i) {
        const DoubanChannel& channel = channels[i];
        QLabel *label = new QLabel(ui->slider);
        QFont font("Sans", 12);
        font.setStyleStrategy(QFont::PreferAntialias);
        label->setFont(font);
        label->setText(channel.name+" MHz");
        label->setStyleSheet(DARK_STYLE);
        label->setMinimumSize(ui->slider->width() / 3, ui->slider->height());
        label->setMaximumSize(ui->slider->width() / 3, ui->slider->height());
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        labels.append(label);
        if (channel.channel_id == this->channel) curindex = i;
        qDebug() << "Channel name=" << qPrintable(channel.name) << " id=" << channel.channel_id;
    }
    ui->slider->setChildren(labels);
    ui->slider->scrollToIndex(curindex);
    ui->slider->currentObject()->setStyleSheet(HIGHTLIGHT_STYLE);

    //pnt->setText(pnt->text().replace("grey", "white").replace("<a>", "<b>").replace("</a>", "</b>"));
}

void ChannelWidget::leaveEvent(QEvent *) {
    emit mouseLeave();
}

void ChannelWidget::wheelEvent(QWheelEvent *ev){
    if(ev->angleDelta().y()<0){
        on_nextButton_clicked();
    }else{
        on_prevButton_clicked();
    }
}
