#include "mainpanel.h"
#include "../interfaces/pluginsiterface.h"
#include <QHBoxLayout>
#include <QLabel>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent),
      m_globalMenuLayout(new QHBoxLayout),
      m_dateTimeLayout(new QHBoxLayout),
      m_pluginManager(new PluginManager)
{
    m_pluginManager->start();

    // QLabel *userNameLabel = new QLabel;
    // QString userName = qgetenv("USER");
    // userNameLabel->setText(qgetenv("USER"));
    // if (userNameLabel->text().isEmpty())
    //     userNameLabel->setText(qgetenv("USERNAME"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addLayout(m_globalMenuLayout);
    layout->addStretch();
    layout->addSpacing(10);
    layout->addLayout(m_dateTimeLayout);
    layout->addSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    loadModules();
}

void MainPanel::loadModules()
{
    loadModule("datetime", m_dateTimeLayout);
}

void MainPanel::loadModule(const QString &pluginName, QHBoxLayout *layout)
{
    QWidget *widget = m_pluginManager->plugin(pluginName)->itemWidget();

    if (widget)
        layout->addWidget(widget);
}