#include "mainwidget.h"
#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>

#ifdef WITH_MPRIS_PLUGIN
#include "plugins/mpris/doubanmprisplugin.h"
#endif

#include "settingdialog.h"

const QString LOCAL_SOCKET_NAME = "QDoubanFM_LocalSocket";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLocalSocket socket;
    socket.connectToServer(LOCAL_SOCKET_NAME);
    if (socket.waitForConnected(500)) {
        qWarning() << "There is already a instance running, raising it up";
        return 0;
    }

    DoubanFM::getInstance();

    MainWidget w;
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

#ifdef WITH_MPRIS_PLUGIN
    new DoubanMprisPlugin();
#endif

    QLocalServer server(&w);
    w.connect(&server, &QLocalServer::newConnection, [&] () {
        if (w.isHidden())
            w.show();
        else
            w.activateWindow();

        qDebug() << "Raise window";
    });
    server.listen(LOCAL_SOCKET_NAME);

    return a.exec();
}
