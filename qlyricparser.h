#ifndef QLYRICPARSER_H
#define QLYRICPARSER_H

#include <QtCore>
#include <QTime>
#include <QList>
#include <QTextStream>

struct QLyric
{
    QTime time;
    QString lyric;
};

typedef QList<QLyric> QLyricList;

class QLyricParser
{
public:
    static QLyricList parse(QTextStream& stream);

private:
    QLyricParser() = delete;
    QLyricParser(const QLyricParser&) = delete;
};

#endif // QLYRICPARSER_H
