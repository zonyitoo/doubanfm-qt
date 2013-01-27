#include <QApplication>
#include "mainui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("豆瓣FM");
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    MainUI ui;
    ui.show();
    
    return a.exec();
}
