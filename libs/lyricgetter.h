#ifndef LYRICGETTER_H
#define LYRICGETTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "qlyricparser.h"
#include "douban_types.h"

class LyricGetter : public QObject {
    Q_OBJECT
public:
    explicit LyricGetter(QObject* parent = 0);
    ~LyricGetter();

    void getLyric(const DoubanFMSong& song);
signals:
    void gotLyric(const QLyricList& lyric);
    void gotLyricError(const QString& errmsg);

private:
    QNetworkAccessManager* querymgr;
};

#endif  // LYRICGETTER_H
