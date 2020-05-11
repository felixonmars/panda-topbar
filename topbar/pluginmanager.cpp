#include "pluginmanager.h"
#include <QDir>
#include <QPluginLoader>
#include <QDebug>

PluginManager::PluginManager(QObject *parent)
  : QObject(parent)
{

}

void PluginManager::start()
{
    QDir pluginsDir("/usr/lib/panda-topbar/plugins");
    const QFileInfoList files = pluginsDir.entryInfoList(QDir::Files);
    for (const QFileInfo file : files) {
      const QString filePath = file.filePath();
      if (!QLibrary::isLibrary(filePath))
          continue;

      QPluginLoader *loader = new QPluginLoader(filePath);
      TopbarPlugin *plugin = qobject_cast<TopbarPlugin *>(loader->instance());

      if (plugin) {
          qDebug() << "load " << plugin->pluginName() << " !!!";
          m_map.insert(plugin->pluginName(), plugin);
      } else {
          qDebug() << filePath << loader->errorString();
      }
    }
}

TopbarPlugin* PluginManager::plugin(const QString &pluginName)
{
    return m_map.value(pluginName, nullptr);
}