#include "doubanfmplugin.h"
#include <QDebug>

DoubanFMPlugin::DoubanFMPlugin(QObject *parent) : QObject(parent), player(DoubanPlayer::getInstance()) {}

DoubanFMPlugin::~DoubanFMPlugin() {}

DoubanFMPluginLoader::DoubanFMPluginLoader(QObject *parent) : QObject(parent) {}

DoubanFMPluginLoader::~DoubanFMPluginLoader() {
    for (auto &p : plugins) {
        delete p;
    }
}

DoubanFMPluginLoader &DoubanFMPluginLoader::getInstance() {
    static DoubanFMPluginLoader instance;
    return instance;
}

void DoubanFMPluginLoader::regPlugin(QString name, const DoubanFMPlugin *plugin) {
    auto itr = plugins.find(name);
    if (itr != plugins.end()) {
        qWarning() << "Plugin name \"" << name << "\" already exists";
    } else {
        plugins[name] = plugin;
    }
}

void DoubanFMPluginLoader::rmPlugin(QString name) { plugins.remove(name); }
