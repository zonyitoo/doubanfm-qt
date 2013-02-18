#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QNetworkReply>
#include "maskpausewidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    _networkmgr = new QNetworkAccessManager(this);

    _channelWidget = new ChannelWidget(this);
    _controlPanel = new ControlPanel(this);
    _mpw = new MaskPauseWidget(this);
    _mpw->setVisible(false);

    _channelWidget->move(this->geometry().x(),
                         -_channelWidget->getInvisibleHeight());
    _controlPanel->move(this->geometry().x(),
                        this->height() - _controlPanel->getVisibleHeight());

    _channelWidget->setChannelId(_controlPanel->getCurrentChannel());

    connect(_channelWidget, SIGNAL(channelChanged(qint32)),
            _controlPanel, SLOT(onChannelChanged(qint32)));
    connect(_networkmgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRecvImage(QNetworkReply*)));
    connect(_controlPanel, SIGNAL(gotAlbumImage(QString)), this, SLOT(onImageChanged(QString)));
    connect(_mpw, SIGNAL(mousePressed()), this, SLOT(onMaskPressed()));

    _userLoginWidget = new UserLoginWidget(this);
    _userLoginWidget->hide();
    connect(_controlPanel, SIGNAL(userButtonClicked()), _userLoginWidget, SLOT(show()));

    pauseShortcut = new QShortcut(QKeySequence("Space"), this);
    connect(pauseShortcut, SIGNAL(activated()), this, SLOT(onPauseShortcutActivate()));
}

MainWidget::~MainWidget()
{
    delete ui;

    delete _channelWidget;
    delete _controlPanel;
    delete _userLoginWidget;
    delete pauseShortcut;
    delete _mpw;
    delete _networkmgr;
}



void MainWidget::onRecvImage(QNetworkReply *reply) {
    if (QNetworkReply::NoError != reply->error()) {
        qDebug() << Q_FUNC_INFO << "pixmap receiving error" << reply->error();
        reply->deleteLater();
        return;
    }
    const QByteArray data(reply->readAll());
    if (!data.size())
        qDebug() << Q_FUNC_INFO << "received pixmap looks like nothing";
    QPixmap pixmap;
    pixmap.loadFromData(data);
    int w = ui->albumImage->width();
    int h = ui->albumImage->height();
    ui->albumImage->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatioByExpanding));
    reply->deleteLater();
}

void MainWidget::onImageChanged(const QString &imgurl) {
    QString mod_url = imgurl;
    mod_url.replace("mpic", "lpic");
    _networkmgr->get(QNetworkRequest(QUrl(mod_url)));
}

void MainWidget::mousePressEvent(QMouseEvent *event) {
    _controlPanel->pause();
    _mpw->setVisible(true);
}

void MainWidget::onMaskPressed() {
    _controlPanel->play();
    _mpw->setVisible(false);
}

void MainWidget::onPauseShortcutActivate() {
    if (_mpw->isVisible()) {
        _mpw->setVisible(false);
        _controlPanel->play();
    }
    else {
        _mpw->setVisible(true);
        _controlPanel->pause();
    }
}
