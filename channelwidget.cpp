#include "channelwidget.h"
#include "ui_channelwidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include "mainwidget.h"

ChannelWidget::ChannelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelWidget)
{
    ui->setupUi(this);

    ui->nextButton->raise();
    ui->prevButton->raise();

    doubanfm = DoubanFM::getInstance();

    connect(doubanfm, SIGNAL(receivedChannels(QList<DoubanChannel>)),
            this, SLOT(setChannels(QList<DoubanChannel>)));
    doubanfm->getChannels();

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
    if (!doubanfm->hasLogin() && channel == -3) channel = 1;
    settings.endGroup();

    connect(doubanfm, &DoubanFM::loginSucceed, [this] (std::shared_ptr<DoubanUser> user) {
        if (ui->slider->currentIndex() != 0)
            this->ui->slider->scrollToIndex(0);
        else
            doubanfm->getNewPlayList(-3);
        qDebug() << "LoginSucceed. Refreshing Playlist";
        QLabel *pnt = static_cast<QLabel *>(labels[0]);
        pnt->setText(pnt->text().replace("grey", "white").replace("<a>", "<b>").replace("</a>", "</b>"));
    });
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::on_nextButton_clicked()
{
    if (ui->slider->currentIndex() == 0) return;
    if (!doubanfm->hasLogin() && ui->slider->currentIndex() == 1) return;
    ui->slider->scrollToIndex(ui->slider->currentIndex() - 1);
    QLabel *pnt = static_cast<QLabel *>(labels[ui->slider->currentIndex()]);
    pnt->setText(pnt->text().replace("white", "grey").replace("<b>", "<a>").replace("</b>", "</a>"));
    pnt = static_cast<QLabel *>(labels[ui->slider->currentIndex() - 1]);
    pnt->setText(pnt->text().replace("grey", "white").replace("<a>", "<b>").replace("</a>", "</b>"));
}

void ChannelWidget::on_prevButton_clicked()
{
    if (ui->slider->currentIndex() == ui->slider->numberOfChildren() - 1) return;
    ui->slider->scrollToIndex(ui->slider->currentIndex() + 1);
    QLabel *pnt = static_cast<QLabel *>(labels[ui->slider->currentIndex()]);
    pnt->setText(pnt->text().replace("white", "grey").replace("<b>", "<a>").replace("</b>", "</a>"));
    pnt = static_cast<QLabel *>(labels[ui->slider->currentIndex() + 1]);
    pnt->setText(pnt->text().replace("grey", "white").replace("<a>", "<b>").replace("</a>", "</b>"));
}

void ChannelWidget::setChannels(const QList<DoubanChannel>& channels) {
    this->channels = channels;
    int curindex = 0;
    for (int i = 0; i < channels.size(); ++ i) {
        const DoubanChannel& channel = channels[i];
        QLabel *label = new QLabel(ui->slider);
        QFont font("文泉驿微米黑", 12);
        font.setStyleStrategy(QFont::PreferAntialias);
        label->setFont(font);
        label->setText(QString("<font color='grey'><a>") +
                        channel.name + QString("</a></font>MHz"));
        label->setMinimumSize(ui->slider->width() / 3, ui->slider->height());
        label->setMaximumSize(ui->slider->width() / 3, ui->slider->height());
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        labels.append(label);
        if (channel.channel_id == this->channel) curindex = i;
    }
    ui->slider->setChildren(labels);
    ui->slider->scrollToIndex(curindex);
    QLabel *pnt = static_cast<QLabel *>(labels[curindex]);
    pnt->setText(pnt->text().replace("grey", "white").replace("<a>", "<b>").replace("</a>", "</b>"));
}

void ChannelWidget::leaveEvent(QEvent *ev) {
    emit mouseLeave();
}
