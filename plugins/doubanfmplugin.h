#ifndef DOUBANFMPLUGIN_H
#define DOUBANFMPLUGIN_H

#include <QObject>
#include <QMap>
#include <libs/doubanplayer.h>

class DoubanFMPlugin : public QObject
{
    Q_OBJECT
public:
    explicit DoubanFMPlugin(QObject *parent = nullptr);
    virtual ~DoubanFMPlugin() = default;

signals:

public slots:

protected:
    DoubanPlayer& player;
};

class DoubanFMPluginLoader : public QObject {
public:
    DoubanFMPluginLoader(QObject *parent = nullptr);
    ~DoubanFMPluginLoader();

    void regPlugin(QString name, const DoubanFMPlugin * plugin);
    void rmPlugin(QString name);

    static DoubanFMPluginLoader & getInstance();
private:
    QMap<QString, const DoubanFMPlugin *> plugins;
};

#define REGISTER_PLUGIN(name, cname) \
    DoubanFMPluginLoader::getInstance().regPlugin(name, new cname())

#define REMOVE_PLUGIN(name) \
    DoubanFMPluginLoader::getInstance().rmPlugin(name)

#endif // DOUBANFMPLUGIN_H
