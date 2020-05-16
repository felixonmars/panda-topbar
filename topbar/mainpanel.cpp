#include "mainpanel.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent),
      m_globalMenuLayout(new QHBoxLayout),
      m_statusnotifierLayout(new QHBoxLayout),
      m_systemTrayLayout(new QHBoxLayout),
      m_dateTimeLayout(new QHBoxLayout),
      m_pluginManager(new PluginManager)
{
    m_pluginManager->start();

    QLabel *userNameLabel = new QLabel;
    QString userName = qgetenv("USER");
    userNameLabel->setText(qgetenv("USER"));
    if (userNameLabel->text().isEmpty())
        userNameLabel->setText(qgetenv("USERNAME"));

    QWidget *statusnotifierWidget = new QWidget;
    statusnotifierWidget->setLayout(m_statusnotifierLayout);
    statusnotifierWidget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_statusnotifierLayout->setMargin(0);

    QWidget *dateTimeWidget = new QWidget;
    dateTimeWidget->setLayout(m_dateTimeLayout);
    dateTimeWidget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_dateTimeLayout->setMargin(0);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addWidget(userNameLabel, 0, Qt::AlignVCenter);
    layout->addSpacing(10);
    layout->addLayout(m_globalMenuLayout);
    layout->addStretch();
    layout->addWidget(statusnotifierWidget, 0, Qt::AlignVCenter);
    layout->addSpacing(3);
    layout->addLayout(m_systemTrayLayout);
    layout->addSpacing(10);
    layout->addWidget(dateTimeWidget, 0, Qt::AlignVCenter);
    layout->addSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    loadModules();
}

void MainPanel::loadModules()
{
    loadModule("datetime", m_dateTimeLayout);
    loadModule("statusnotifier", m_statusnotifierLayout);
    loadModule("systemtray", m_systemTrayLayout);
}

void MainPanel::loadModule(const QString &pluginName, QHBoxLayout *layout)
{
    TopbarPlugin *plugin = m_pluginManager->plugin(pluginName);

    if (plugin) {
        QWidget *widget = plugin->itemWidget();
        if (widget) {
            qDebug() << pluginName << " add to panel";
            layout->addWidget(widget);
        }
    }
}