#ifndef DOUBANMPRISPLUGIN_H
#define DOUBANMPRISPLUGIN_H

#include <QObject>
#include "libs/doubanplayer.h"
#include <plugins/doubanfmplugin.h>
#include <QDBusMessage>
#include <memory>

class DoubanMprisPlugin : public DoubanFMPlugin
{
    Q_OBJECT
public:
    explicit DoubanMprisPlugin(QObject *parent = 0);

    Q_PROPERTY(bool CanControl READ CanControl)
    bool CanControl() const;
    Q_PROPERTY(bool CanGoNext READ CanGoNext)
    bool CanGoNext() const;
    Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious)
    bool CanGoPrevious() const;
    Q_PROPERTY(bool CanPause READ CanPause)
    bool CanPause() const;
    Q_PROPERTY(bool CanSeek READ CanSeek)
    bool CanSeek() const;
    Q_PROPERTY(qlonglong Position READ Position)
    qlonglong Position() const;
    Q_PROPERTY(QVariantMap Metadata READ Metadata)
    QVariantMap Metadata() const;
    Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus)
    QString PlaybackStatus() const;

    Q_PROPERTY(bool CanQuit READ CanQuit)
    bool CanQuit() const;
    Q_PROPERTY(bool CanRaise READ CanRaise)
    bool CanRaise() const;
    Q_PROPERTY(QString DesktopEntry READ DesktopEntry)
    QString DesktopEntry() const;
    Q_PROPERTY(QString Identity READ Identity)
    QString Identity() const;

signals:

public slots:
    void Next();
    void Pause();
    void Play();
    void PlayPause();
    void Stop();

    void Quit();
    void Raise();
};

#endif // DOUBANMPRISPLUGIN_H
