#ifndef PLUGINSINTERFACE_H
#define PLUGINSINTERFACE_H

#include <QtPlugin>

class TopbarPlugin
{
public:
    virtual ~TopbarPlugin() {}

    virtual QString pluginName() = 0;
    virtual QString displayName() { return QString(); }
    virtual QWidget *itemWidget() = 0;
};

Q_DECLARE_INTERFACE(TopbarPlugin, "org.panda.topbar/1.0")

#endif
