#include "mainpanel.h"
#include "datetimewidget.h"
#include <QHBoxLayout>
#include <QLabel>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent),
      m_appMenuWidget(new AppMenuWidget),
      m_trayWidget(new TrayWidget)
      //m_volumeWidget(new VolumeWidget)
{
    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(QPixmap(":/resources/logo.svg"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(15);
    layout->addWidget(logoLabel);
    layout->addSpacing(15);
    layout->addWidget(m_appMenuWidget);
    layout->addStretch();
    layout->addWidget(m_trayWidget, 0, Qt::AlignVCenter);
    layout->addSpacing(5);
    // layout->addWidget(m_volumeWidget);
    layout->addWidget(new DateTimeWidget, 0, Qt::AlignVCenter);
    layout->addSpacing(5);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}
