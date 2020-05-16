#ifndef APPMENUMODEL_H
#define APPMENUMODEL_H

#include <QAbstractListModel>
#include <QAbstractNativeEventFilter>
#include <QStringList>
#include <KWindowSystem>
#include <QPointer>
#include <QRect>

class QMenu;
class QModelIndex;
class QDBusServiceWatcher;
class KDBusMenuImporter;

class AppMenuModel : public QAbstractListModel, public QAbstractNativeEventFilter
{
    Q_OBJECT

    Q_PROPERTY(bool menuAvailable READ menuAvailable WRITE setMenuAvailable NOTIFY menuAvailableChanged)
    Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)

    Q_PROPERTY(QRect screenGeometry READ screenGeometry WRITE setScreenGeometry NOTIFY screenGeometryChanged)

public:
    explicit AppMenuModel(QObject *parent = nullptr);
    ~AppMenuModel() override;

    enum AppMenuRole {
        MenuRole = Qt::UserRole+1, // TODO this should be Qt::DisplayRole
        ActionRole
    };

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void updateApplicationMenu(const QString &serviceName, const QString &menuObjectPath);

    bool menuAvailable() const;
    void setMenuAvailable(bool set);

    bool visible() const;

    QRect screenGeometry() const;
    void setScreenGeometry(QRect geometry);

signals:
    void requestActivateIndex(int index);

protected:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long int *result) override;

private Q_SLOTS:
    void onActiveWindowChanged(WId id);
    void onWindowChanged(WId id);
    void setVisible(bool visible);
    void update();

signals:
    void menuAvailableChanged();
    void modelNeedsUpdate();
    void screenGeometryChanged();
    void visibleChanged();

private:
    bool m_menuAvailable;
    bool m_updatePending = false;
    bool m_visible = true;

    QRect m_screenGeometry;

    //! current active window used
    WId m_currentWindowId = 0;
    //! window that its menu initialization may be delayed
    WId m_delayedMenuWindowId = 0;

    QPointer<QMenu> m_menu;

    QDBusServiceWatcher *m_serviceWatcher;
    QString m_serviceName;
    QString m_menuObjectPath;

    QPointer<KDBusMenuImporter> m_importer;
};

#endif
