#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include "tray/traywidget.h"
#include "appmenu/appmenuwidget.h"
// #include "volumewidget.h"

class MainPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);

private:
    AppMenuWidget *m_appMenuWidget;
    TrayWidget *m_trayWidget;
    //VolumeWidget *m_volumeWidget;
};

#endif // MAINPANEL_H
