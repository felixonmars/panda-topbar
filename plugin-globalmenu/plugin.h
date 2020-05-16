#ifndef PLUGIN_H
#define PLUGIN_H

#include "pluginsiterface.h"
#include "appmenuwidget.h"
#include <QObject>

class Plugin : public QObject, TopbarPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.panda.topbar/1.0")
    Q_INTERFACES(TopbarPlugin)

public:
    explicit Plugin(QObject *parent = nullptr) : QObject(parent) {}

    QString pluginName() override { return "globalmenu"; }
    QString displayName() override { return "Global Menu"; }
    QWidget *itemWidget() override { return new AppMenuWidget; }
};

#endif
