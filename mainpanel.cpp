#include "mainpanel.h"
#include "datetimewidget.h"
#include <QHBoxLayout>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent),
      m_trayWidget(new TrayWidget),
      m_volumeWidget(new VolumeWidget)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(m_trayWidget, 0, Qt::AlignVCenter);
    layout->addSpacing(5);
    layout->addWidget(m_volumeWidget);
    layout->addWidget(new DateTimeWidget, 0, Qt::AlignVCenter);
    setLayout(layout);
}
