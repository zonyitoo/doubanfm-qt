#include "mainwidget.h"
#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>
#include <QTranslator>
#include <QLocale>

#include "plugins/plugin.hpp"

#include "settingdialog.h"

const QString LOCAL_SOCKET_NAME = "QDoubanFM_LocalSocket";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (true) {
        auto translator = new QTranslator(&a);
        translator->load("zh_CN", QApplication::applicationDirPath() + "/i18n");
        a.installTranslator(translator);
        //此地暂作这样的处理，以后可以增加语言切换功能
    }
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

    load_plugins();

    QLocalServer server(&w);
    w.connect(&server, &QLocalServer::newConnection, [&] () {
        if (w.isHidden())
            w.show();
        else
            w.activateWindow();

        qDebug() << "Raise window";
    });
    server.listen(LOCAL_SOCKET_NAME);

    // i18n
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("i18n/" + QLocale::system().name());
    a.installTranslator(&myappTranslator);

    return a.exec();
}
