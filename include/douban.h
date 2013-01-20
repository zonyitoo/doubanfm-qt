#ifndef DOUBAN_H
#define DOUBAN_H

#include <QObject>
#include <QNetworkAccessManager>

struct DoubanUser {
    QString user_id;
    QString expire;
    QString token;
};

struct DoubanChannel {
    QString name;
    qint32 seq_id;
    QString abbr_en;
    qint32 channel_id;
    QString name_en;
};

struct DoubanFMSong {
    QString album;
    QString picture;
    QString ssid;
    QString artist;
    QString url;
    QString company;
    QString title;
    double rating_avg;
    quint32 length;
    QString subtype;
    QString public_time;
    quint32 sid;
    quint32 aid;
    quint32 kbps;
    QString albumtitle;
    bool like;
};

class Douban : public QObject
{
    Q_OBJECT
public:
    ~Douban();
    static Douban* getInstance(const DoubanUser& user = DoubanUser());

    DoubanUser userLogin(const QString& name, const QString& password);
    void userLogout();

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
    
signals:
    void receivedNewList(const QList<DoubanFMSong>& songs);
    void receivedPlayingList(const QList<DoubanFMSong>& songs);
    void receivedRateSong(const bool succeed);
    void receivedSkipSong(const bool succeed);
    void receivedCurrentEnd(const bool succeed);
    void receivedByeSong(const bool succeed);
    void receivedChannels(const QList<DoubanChannel>& channels);
    
private slots:
    void onReceivedNewList(QNetworkReply *reply);
    void onReceivedPlayingList(QNetworkReply *reply);
    void onReceivedRateSong(QNetworkReply *reply);
    void onReceivedSkipSong(QNetworkReply *reply);
    void onReceivedCurrentEnd(QNetworkReply *reply);
    void onReceivedByeSong(QNetworkReply *reply);
    void onReceivedChannels(QNetworkReply *reply);

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
};



#endif // DOUBAN_H
