#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QMap>
#include "../interfaces/pluginsiterface.h"

class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(QObject *parent = 0);

    void start();
    TopbarPlugin *plugin(const QString &pluginName);

private:
    QMap<QString, TopbarPlugin *> m_map;
};

#endif