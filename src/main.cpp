#include <QApplication>
#include "mainwidget.h"
#include <QTranslator>
#include <QResource>
#include <QDebug>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("DoubanFM");
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //QResource::registerResource("icons.qrc");

    QTranslator translator;
    QLocale locale;
    translator.load(QString(":/lang/")
                    + QLocale::countryToString(locale.country())
                    + QString("_")
                    + QLocale::languageToString(locale.language()));
    qDebug() << QString("Load Language: ")
                + QLocale::countryToString(locale.country())
                + QString("_")
                + QLocale::languageToString(locale.language());

    a.installTranslator(&translator);

    MainWidget mw;
    mw.show();
    
    return a.exec();
}
