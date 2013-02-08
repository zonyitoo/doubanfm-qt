#include <QApplication>
#include "mainui.h"
#include "mainwidget.h"
#include <QTranslator>
#include <QResource>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("豆瓣FM");
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //QResource::registerResource("icons.qrc");

    //MainUI ui;
    //ui.show();
    MainWidget mw;
    mw.show();
    
    return a.exec();
}
