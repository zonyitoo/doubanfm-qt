#ifndef DOUBAN_H
#define DOUBAN_H

#include <QObject>
#include <QNetworkAccessManager>
#include "douban_types.h"
#include "doubanlogindialog.h"

class Douban : public QObject
{
    Q_OBJECT
public:
    ~Douban();
    static Douban* getInstance(const DoubanUser& user = DoubanUser());

    void userLogin();
    void doLogin(const QString &email, const QString &password);
    void userLogout();
    void userReLogin();

    void setLoginDialog(DoubanLoginDialog *dialog);

    void getNewPlayList(const quint32& channel);
    void getPlayingList(const quint32& channel, const quint32& sid);
    void rateSong(const quint32& sid, const quint32& channel, const bool toRate);
    void unrateSong(const quint32& sid, const quint32& channel);
    void skipSong(const quint32& sid, const quint32& channel);
    void songEnd(const quint32& sid, const quint32& channel);
    void byeSong(const quint32& sid, const quint32& channel);

    void getChannels();

    void setUser(const DoubanUser& user);
    DoubanUser getUser();
    bool hasLogin();
    
signals:
    void receivedNewList(const QList<DoubanFMSong>& songs);
    void receivedPlayingList(const QList<DoubanFMSong>& songs);
    void receivedRateSong(const bool succeed);
    void receivedSkipSong(const bool succeed);
    void receivedCurrentEnd(const bool succeed);
    void receivedByeSong(const bool succeed);
    void receivedChannels(const QList<DoubanChannel>& channels);
    void loginSucceed(DoubanUser *user);
    void logoffSucceed();
    
private slots:
    void onReceivedAuth(QNetworkReply *reply);
    void onReceivedNewList(QNetworkReply *reply);
    void onReceivedPlayingList(QNetworkReply *reply);
    void onReceivedRateSong(QNetworkReply *reply);
    void onReceivedSkipSong(QNetworkReply *reply);
    void onReceivedCurrentEnd(QNetworkReply *reply);
    void onReceivedByeSong(QNetworkReply *reply);
    void onReceivedChannels(QNetworkReply *reply);

    void onLoginSucceed(DoubanUser user);

private:
    explicit Douban(QObject *parent = 0);
    /**
     * @brief _managers
     *
     * 0: User login/logout
     * 1: new list
     * 2: rate song/unrate song
     * 3: skip song
     * 4: current end
     * 5: bye song
     * 6: get channels
     * 7: playing list
     */
    QNetworkAccessManager *_managers[8];

    DoubanUser _user;

    DoubanLoginDialog *loginDialog;
};



#endif // DOUBAN_H
