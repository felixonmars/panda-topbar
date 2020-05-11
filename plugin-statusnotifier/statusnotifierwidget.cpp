#include "statusnotifierwidget.h"
#include <QApplication>
#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDBusConnectionInterface>

StatusNotifierWidget::StatusNotifierWidget(QWidget *parent) 
  : QWidget(parent),
    m_layout(new QHBoxLayout(this))
{
    m_layout->setSpacing(0);
    m_layout->setMargin(0);

    QFutureWatcher<StatusNotifierWatcher *> * future_watcher = new QFutureWatcher<StatusNotifierWatcher *>;
    connect(future_watcher, &QFutureWatcher<StatusNotifierWatcher *>::finished, this, [this, future_watcher]
        {
            mWatcher = future_watcher->future().result();

            connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemRegistered,
                    this, &StatusNotifierWidget::itemAdded);
            connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemUnregistered,
                    this, &StatusNotifierWidget::itemRemoved);

            qDebug() << mWatcher->RegisteredStatusNotifierItems();

            future_watcher->deleteLater();
        });

    QFuture<StatusNotifierWatcher *> future = QtConcurrent::run([]
        {
            QString dbusName = QStringLiteral("org.kde.StatusNotifierHost-%1-%2").arg(QApplication::applicationPid()).arg(1);
            if (QDBusConnectionInterface::ServiceNotRegistered == QDBusConnection::sessionBus().interface()->registerService(dbusName, QDBusConnectionInterface::DontQueueService))
                qDebug() << "unable to register service for " << dbusName;

            StatusNotifierWatcher * watcher = new StatusNotifierWatcher;
            watcher->RegisterStatusNotifierHost(dbusName);
            watcher->moveToThread(QApplication::instance()->thread());
            return watcher;
        });

    future_watcher->setFuture(future);
}

StatusNotifierWidget::~StatusNotifierWidget()
{
    delete mWatcher;
}

void StatusNotifierWidget::itemAdded(QString serviceAndPath)
{
    int slash = serviceAndPath.indexOf(QLatin1Char('/'));
    QString serv = serviceAndPath.left(slash);
    QString path = serviceAndPath.mid(slash);
    StatusNotifierButton *button = new StatusNotifierButton(serv, path, this);
    button->setFixedSize(24, 24);

    mServices.insert(serviceAndPath, button);
    m_layout->addWidget(button);
    button->show();
}

void StatusNotifierWidget::itemRemoved(const QString &serviceAndPath)
{
    StatusNotifierButton *button = mServices.value(serviceAndPath, nullptr);
    if (button) {
        button->deleteLater();
        m_layout->removeWidget(button);
    }
}