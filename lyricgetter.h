#ifndef LYRICGETTER_H
#define LYRICGETTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "qlyricparser.h"

class LyricGetter : public QObject
{
    Q_OBJECT
public:
    explicit LyricGetter(QObject *parent = 0);
    ~LyricGetter();
    
    void getLyric(const QString& song, const QString& artist);
signals:
    void gotLyric(const QLyricList& lyric);
private:
    QNetworkAccessManager *querymgr;
    QNetworkAccessManager *getmgr;
};

#endif // LYRICGETTER_H
