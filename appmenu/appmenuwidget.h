#ifndef APPMENUWIDGET_H
#define APPMENUWIDGET_H

#include <QAbstractNativeEventFilter>
#include <QPointer>
#include <QWidget>
#include <QLabel>
#include <QIcon>
#include <QSettings>
#include <dbusmenuimporter.h>

#include "appmenudbus.h"
#include "menuimporter.h"

class QDBusMenuImporter : public DBusMenuImporter
{

public:
    QDBusMenuImporter(const QString &service, const QString &path, QObject *parent)
        : DBusMenuImporter(service, path, parent)
    {
    }

protected:
    QIcon iconForName(const QString &name) override
    {
        return QIcon::fromTheme(name);
    }
};

class QMenuBar;
class AppMenuWidget : public QWidget, public QAbstractNativeEventFilter, protected QDBusContext
{
    Q_OBJECT

public:
    explicit AppMenuWidget(QWidget *parent = nullptr);

private:
    void reconfigure();
    void onActiveWindowChanged(WId id);
    void itemActivationRequested(int actionId, uint timeStamp);

    void slotWindowRegistered(WId id, const QString &serviceName, const QDBusObjectPath &menuObjectPath);
    void updateApplicationMenu(const QString &serviceName, const QString &menuObjectPath);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long int *result) override;

private:
    AppmenuDBus *m_appMenuDBus;
    MenuImporter *m_menuImporter = nullptr;
    WId m_delayedMenuWindowId = 0;

    QDBusServiceWatcher *m_serviceWatcher;
    QString m_serviceName;
    QString m_menuObjectPath;

    QPointer<QDBusMenuImporter> m_importer;
    QMenuBar *m_menuBar;
};

#endif // DATETIMEWIDGET_H
