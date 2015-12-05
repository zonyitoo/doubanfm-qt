#include "qlyricparser.h"
#include <QRegExp>
#include <QDebug>
#include <algorithm>

QLyricList QLyricParser::parse(QTextStream& stream) {
    // [00:00.00] or [00:00.000] or [00:00]
    QRegularExpression timeRegExp("\\[([0-9]{2}):([0-9]{2})\\.([0-9]{2,3})\\]|\\[([0-9]{2}):([0-9]{2})\\]");
    QList<QLyric> result;
    int zeroCount = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QList<QTime> ticks;
        QRegularExpressionMatch match = timeRegExp.match(line);
        int length = 0;
        while (match.hasMatch()) {
            length          = length + match.captured(0).size();
            QString minute  = match.captured(1);
            QString second  = match.captured(2);
            QString msecond = match.captured(3);

            if (msecond.isEmpty()) {
                // This is a flag to determine if the time format is [00:00]. It will be used afterwords.
                QTime time(0, match.captured(4).toInt(), match.captured(5).toInt(), 0);
                ticks.append(time);
            } else {
                QTime time(0, minute.toInt(), second.toInt(), msecond.toInt());
                ticks.append(time);
            }
            match = timeRegExp.match(line, length);
        }
        if (ticks.size() != 0)
            zeroCount++;
        QString lyricStr = line.right(line.size() - length);
        for (const QTime& t : ticks) {
            QLyric lyric;
            lyric.time  = t;
            lyric.lyric = lyricStr;
            result.append(lyric);
        }
    }
    std::sort(result.begin(), result.end(), [](const QLyric& a, const QLyric& b) -> bool { return a.time < b.time; });

    return result;
}
