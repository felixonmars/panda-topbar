#ifndef STATUSNOTIFIER_H
#define STATUSNOTIFIER_H

#include "pluginsiterface.h"
#include <QObject>

class Statusnotifier : public QObject, TopbarPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.panda.topbar/1.0")
    Q_INTERFACES(TopbarPlugin)

public:
    explicit Statusnotifier(QObject *parent = nullptr);

    QString pluginName() override { return "statusnotifier"; }
    QString displayName() override { return QString(); }
    QWidget *itemWidget() override { return nullptr; }
};

#endif
