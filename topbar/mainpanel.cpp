#include "mainpanel.h"
#include "datetimewidget.h"
#include <QHBoxLayout>
#include <QLabel>

#include <QDir>
#include <QPluginLoader>
#include "../interfaces/pluginsiterface.h"

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

    QDir pluginsDir("/usr/lib/panda-topbar/plugins");
    const QFileInfoList files = pluginsDir.entryInfoList(QDir::Files);
    for (const QFileInfo file : files) {
        const QString filePath = file.filePath();
        if (!QLibrary::isLibrary(filePath))
            continue;

        QPluginLoader *loader = new QPluginLoader(filePath);
        const QJsonObject &meta = loader->metaData().value("MetaData").toObject();
        const QString &pluginApi = meta.value("api").toString();

        TopbarPlugin *interface = qobject_cast<TopbarPlugin *>(loader->instance());

        if (interface) {
            qDebug() << "load " << interface->pluginName() << " !!!";
        } else {
            qDebug() << filePath << loader->errorString();
        }
    }
}
