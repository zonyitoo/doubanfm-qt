#include "lyricwidget.h"
#include "libs/doubanplayer.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QPropertyAnimation>
#include <QDebug>


LyricWidget::LyricWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui_LyricWidget),
    animWidget(nullptr),
    firstShowing(false),
    lyricGetter(new LyricGetter(this)),
    isShowing(false), haveSearchedLyric(false), saveTick(0)
{
    ui->setupUi(this);
    ui->border->setText(QString("<font color='grey'>") + tr("No lyrics") + QString("</font>"));
    ui->bg->lower();

    connect(&DoubanPlayer::getInstance(), SIGNAL(positionChanged(qint64)),
            this, SLOT(setTick(qint64)));
    connect(lyricGetter, &LyricGetter::gotLyric, [this] (const QLyricList& lyric) {
        this->setLyric(lyric);
    });
    connect(lyricGetter, &LyricGetter::gotLyricError, [this] (const QString&) {
        this->clear();
    });
    connect(&DoubanPlayer::getInstance(), SIGNAL(currentSongChanged(DoubanFMSong)),
            this, SLOT(setSong(DoubanFMSong)));
}

LyricWidget::~LyricWidget() {
    if (animWidget) delete animWidget;
    delete lyricGetter;
    delete ui;
}

void LyricWidget::setLyric(const QLyricList &lyric) {
    if (animWidget) delete animWidget;
    animWidget = new QWidget(this);
    animWidget->raise();
    ui->border->raise();
    QRect geo = this->geometry();
    animWidget->setMinimumWidth(geo.width());
    animWidget->setMaximumWidth(geo.width());
    animWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QVBoxLayout *vbox = new QVBoxLayout(animWidget);
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->setContentsMargins(0, 0, 0, 0);
    animWidget->setLayout(vbox);

    labels.clear();
    curInd = 0;
    this->lyric = lyric;
    firstShowing = false;

    int widgetWidth = this->geometry().width();
    int accuHeight = 0;
    for (const QLyric& lyr : lyric) {
        QLabel *label = new QLabel(animWidget);
        QFont font("文泉驿微米黑", 11);
        font.setStyleStrategy(QFont::PreferAntialias);
        label->setFont(font);
        label->setText(QString("<font color='grey'>") +
                        lyr.lyric + QString("</font>"));
        label->setMaximumWidth(widgetWidth);
        label->setMaximumWidth(widgetWidth);
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        label->setWordWrap(true);
        labels.append(label);
        QRect fsize = label->fontMetrics().boundingRect(label->text());
        int height = (widgetWidth + fsize.width()) / widgetWidth * fsize.height() + 15;
        heights.append(height);
        label->setMinimumHeight(height);
        label->setMaximumHeight(height);
        accuHeight += height;

        animWidget->layout()->addWidget(label);
    }

    if (heights.size() > 0) {
        animWidget->setGeometry(0, this->geometry().height() / 2 + heights[0],
                                this->geometry().width(), accuHeight);
    }
    animWidget->show();
    ui->border->setText("");
}

void LyricWidget::setTick(qint64 tick) {
    if (!isShowing) {
        saveTick = tick;
        return;
    }
    if (tick != 0) {
        QTime time(0, (tick / 60000) % 60, (tick / 1000) % 60, tick % 1000);
        setTime(time);
    }
    else
        setTime(QTime(0, 0, 0, 0));
}

void LyricWidget::setTime(const QTime &time) {
    if (lyric.size() == 0) return;
    int befind = curInd;
    if (time < lyric[curInd].time) {
        for (int i = curInd - 1; i >= 0; -- i) {
            if (lyric[i].time <= time && lyric[i + 1].time > time) {
                curInd = i;
                break;
            }
        }
    }
    else {
        if (curInd == lyric.size() - 2 && time >= lyric[lyric.size() - 1].time)
            curInd ++;
        else if (curInd < lyric.size() - 2) {
            for (int i = curInd; i < lyric.size(); ++ i) {
                if (lyric[i].time > time) {
                    curInd = i > 0 ? i - 1 : 0;
                    break;
                }
            }
        }
    }
    if (curInd == 0 && !firstShowing) firstShowing = true;
    else if (curInd == befind) return;

    QPropertyAnimation *anim = new QPropertyAnimation(animWidget, "geometry");
    anim->setDuration(400);
    anim->setStartValue(animWidget->geometry());
    int accuHeight = 0;
    for (int i = 0; i < curInd; ++ i) accuHeight += heights[i];
    accuHeight += heights[curInd] / 2;
    accuHeight = this->height() / 2 - accuHeight;
    QRect endval(0, accuHeight, animWidget->width(), animWidget->height());
    anim->setEndValue(endval);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    labels[befind]->setText(labels[befind]->text().replace("white", "grey"));
    connect(anim, &QPropertyAnimation::finished, [this] () {
        labels[curInd]->setText(labels[curInd]->text().replace("grey", "white"));
    });
    anim->start(QPropertyAnimation::DeleteWhenStopped);
}

void LyricWidget::clear() {
    if (animWidget) {
        animWidget->hide();
        delete animWidget;
        animWidget = nullptr;
    }
    curInd = 0;
    lyric.clear();
    labels.clear();
    heights.clear();
    firstShowing = false;
    ui->border->setText(QString("<font color='grey'>") + tr("No lyrics") + QString("</font>"));
}

void LyricWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

void LyricWidget::setSong(const DoubanFMSong &song) {
    this->clear();
    if (isShowing) {
        lyricGetter->getLyric(song);
        this->haveSearchedLyric = true;
    }
    else {
        this->saveCurrentSong = song;
        this->haveSearchedLyric = false;
    }
}

void LyricWidget::setShowing(bool s) {
    isShowing = s;
    if (s) {
        if (!haveSearchedLyric) {
            this->haveSearchedLyric = true;
            lyricGetter->getLyric(saveCurrentSong);
        }
        else {
            this->setTick(saveTick);
        }
    }
}
