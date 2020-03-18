#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QWidget>
#include "tray/traywidget.h"

class MainPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);

private:
    TrayWidget *m_trayWidget;
};

#endif // MAINPANEL_H
