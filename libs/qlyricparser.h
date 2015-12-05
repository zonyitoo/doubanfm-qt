#ifndef QLYRICPARSER_H
#define QLYRICPARSER_H

#include <QtCore>
#include <QTime>
#include <QList>
#include <QTextStream>

struct QLyric {
    QTime time;
    QString lyric;
};

typedef QList<QLyric> QLyricList;

class QLyricParser {
public:
    static QLyricList parse(QTextStream& stream);

private:
#ifdef MSVC2012
    QLyricParser();
    QLyricParser(const QLyricParser&);
#else  // vs2012 not support the feature of c++11 now
    QLyricParser() = delete;
    QLyricParser(const QLyricParser&) = delete;
#endif
};

#endif  // QLYRICPARSER_H
