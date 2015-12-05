#ifndef DOUBAN_H
#define DOUBAN_H

#include <QObject>
#include <QNetworkAccessManager>
#include "libs/douban_types.h"
#include <memory>

static const unsigned int DOUBAN_MANAGER_ARRAY_SIZE = 9;

class DoubanFM : public QObject {
    Q_OBJECT
public:
    ~DoubanFM();
    static DoubanFM& getInstance();

    void userLogin(const QString& email, const QString& password);
    void userLogout();
    void userReLogin();

    void getNewPlayList(const qint32& channel, qint32 kbps = 64);
    void getPlayingList(const qint32& channel, const quint32& sid, qint32 kbps = 64);
    void rateSong(const quint32& sid, const qint32& channel, const bool toRate);
    void unrateSong(const quint32& sid, const qint32& channel);
    void skipSong(const quint32& sid, const qint32& channel);
    void songEnd(const quint32& sid, const qint32& channel);
    void byeSong(const quint32& sid, const qint32& channel);

    void getChannels();

    void setUser(const DoubanUser& user);
    const DoubanUser* getUser() const;
    bool hasLogin();

signals:
    void receivedNewList(const QList<DoubanFMSong>& songs);
    void receivedPlayingList(const QList<DoubanFMSong>& songs);
    void receivedRateSong(bool succeed);
    void receivedSkipSong(bool succeed);
    void receivedCurrentEnd(bool succeed);
    void receivedByeSong(bool succeed);
    void receivedChannels(const QList<DoubanChannel>& channels);
    void loginSucceed(const DoubanUser& user);
    void loginFailed(const QString& errmsg);
    void logoffSucceed();

private slots:
    void onReceivedAuth(QNetworkReply* reply);
    void onReceivedRelogin(QNetworkReply* reply);
    void onReceivedNewList(QNetworkReply* reply);
    void onReceivedPlayingList(QNetworkReply* reply);
    void onReceivedRateSong(QNetworkReply* reply);
    void onReceivedSkipSong(QNetworkReply* reply);
    void onReceivedCurrentEnd(QNetworkReply* reply);
    void onReceivedByeSong(QNetworkReply* reply);
    void onReceivedChannels(QNetworkReply* reply);

private:
    explicit DoubanFM(QObject* parent = 0);
    /**
     * @brief _managers
     *
     * 0: User Relogin
     * 1: new list
     * 2: rate song/unrate song
     * 3: skip song
     * 4: current end
     * 5: bye song
     * 6: get channels
     * 7: playing list
     * 8: Login/Logout
     */
    QNetworkAccessManager* _managers[DOUBAN_MANAGER_ARRAY_SIZE];

    std::unique_ptr<DoubanUser> _user;
};

#endif // DOUBAN_H
