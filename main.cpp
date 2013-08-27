#include "mainwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mainwidget w;
    //w.setWindowFlags(Qt::FramelessWindowHint);
    //w.setAttribute(Qt::WA_NoBackground);
    //w.setAttribute(Qt::WA_NoSystemBackground);
    //w.setAttribute(Qt::WA_TranslucentBackground);

    w.show();
    QFont appfont = QApplication::font();
    appfont.setStyleStrategy(QFont::PreferAntialias);
    a.setFont(appfont);

    return a.exec();
}
