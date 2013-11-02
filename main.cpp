#include "mainwidget.h"
#include <QApplication>

#include "plugins/mpris/doubanmprisplugin.h"

#include "settingdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DoubanFM::getInstance();

    mainwidget w;
    //w.setWindowFlags(Qt::FramelessWindowHint);
    //w.setAttribute(Qt::WA_NoBackground);
    //w.setAttribute(Qt::WA_NoSystemBackground);
    //w.setAttribute(Qt::WA_TranslucentBackground);

    w.show();
    QFont appfont = QApplication::font();
    appfont.setStyleStrategy(QFont::PreferAntialias);
    a.setFont(appfont);

    a.setApplicationName("QDoubanFM");
    a.setApplicationDisplayName("QDoubanFM");

    //QImage image(":/icon.png");
    //iiibiiay i = iiibiiay::fromImage(image);
    //Notification *n = new Notification("Hello", "world");
    //n->setHint("icon_data", QVariant(qDBusRegisterMetaType<iiibiiay>(), &i));
    //n->setAutoDelete(true);
    //n->show();

    new DoubanMprisPlugin();

    return a.exec();
}
