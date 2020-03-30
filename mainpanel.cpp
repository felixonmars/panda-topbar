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
    QLabel *userNameLabel = new QLabel;
    //userNameLabel->setPixmap(QPixmap(":/resources/logo.svg"));

    QString userName = qgetenv("USER");
    userNameLabel->setText(qgetenv("USER"));
    if (userNameLabel->text().isEmpty())
        userNameLabel->setText(qgetenv("USERNAME"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addWidget(userNameLabel);
    layout->addSpacing(10);
    layout->addWidget(m_appMenuWidget);
    layout->addStretch();
    layout->addWidget(m_trayWidget, 0, Qt::AlignVCenter);
    layout->addSpacing(10);
    // layout->addWidget(m_volumeWidget);
    layout->addWidget(new DateTimeWidget, 0, Qt::AlignVCenter);
    layout->addSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}
