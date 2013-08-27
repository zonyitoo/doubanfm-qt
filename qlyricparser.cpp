#include "qlyricparser.h"
#include <QRegExp>
#include <QDebug>
#include <algorithm>

QLyricList QLyricParser::parse(QTextStream &stream) {
    QRegExp timeExp;
    timeExp.setPatternSyntax(QRegExp::RegExp);
    timeExp.setCaseSensitivity(Qt::CaseSensitive);
    timeExp.setPattern("\\[([0-9]{2}):([0-9]{2})\.([0-9]{2})\\]");

    QList<QLyric> result;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        int ret = timeExp.indexIn(line);
        QList<QTime> ticks;
        int lastindex = 0;
        while (ret >= 0) {
            QStringList tstr = timeExp.capturedTexts();
            QTime time(0, tstr[1].toInt(), tstr[2].toInt(), tstr[3].toInt());
            ticks.append(time);
            lastindex = ret + timeExp.matchedLength();
            ret = timeExp.indexIn(line, lastindex);
        }
        QString lyricstr = line.right(line.size() - lastindex);
        for (const QTime& t : ticks) {
            QLyric lyric;
            lyric.time = t;
            lyric.lyric = lyricstr;
            result.append(lyric);
        }
    }
    std::sort(result.begin(), result.end(), [] (const QLyric& a, const QLyric& b) -> bool {
        return a.time < b.time;
    });

    return result;
}
