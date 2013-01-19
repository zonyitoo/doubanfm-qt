#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QtNetwork/QtNetwork>
#include <QtGui>
#include "doubanuser.h"
#include "doubanfmsong.h"

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    NetworkManager(QWidget *widget);
    virtual ~NetworkManager();

    void getNewList(const int channel,
                    const DoubanUser& user = DoubanUser());
    void getPlayingList(const int channel,
                        const QList<DoubanFMSong> &historyList,
                        const DoubanUser& user = DoubanUser());
    void rateSong(const int channel,
                  const QString& sid,
                  const DoubanUser& user = DoubanUser());
    void unrateSong(const int channel,
                    const QString& sid,
                    const DoubanUser& user = DoubanUser());
    void skipSong(const int channel,
                  const QString& sid,
                  const DoubanUser& user = DoubanUser());
    void currentEnd(const int channel,
                    const QString& sid,
                    const DoubanUser& user = DoubanUser());
    void byeSong(const int channel,
                 const QString& sid,
                 const DoubanUser& user = DoubanUser());

    void getImage(const QString &url);

signals:
    void receivedNewList(const QList<DoubanFMSong>& songs);
    void receivedPlayingList(const QList<DoubanFMSong>& songs);
    void receivedRateSong(const QList<DoubanFMSong>& songs);
    void receivedUnrateSong(const QList<DoubanFMSong>& songs);
    void receivedSkipSong(const QList<DoubanFMSong>& songs);
    void receivedCurrentEnd(const QList<DoubanFMSong>& songs);
    void receivedByeSong(const QList<DoubanFMSong>& songs);
    void receivedImage(const QByteArray &data);

private:
    QNetworkAccessManager *_manager;

private slots:
    void onReceivedNewList(QNetworkReply *reply);
    void onReceivedPlayingList(QNetworkReply *reply);
    void onReceivedRateSong(QNetworkReply *reply);
    void onReceivedUnrateSong(QNetworkReply *reply);
    void onReceivedSkipSong(QNetworkReply *reply);
    void onReceivedCurrentEnd(QNetworkReply *reply);
    void onReceivedByeSong(QNetworkReply *reply);
    void onReceivedImage(QNetworkReply *reply);
};

#endif // NETWORKMANAGER_H
