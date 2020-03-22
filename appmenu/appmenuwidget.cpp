#include "appmenuwidget.h"
#include <KWindowSystem>
#include <QDebug>
#include <QMenu>

#include <QApplication>
#include <QX11Info>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusServiceWatcher>

#include <QHBoxLayout>
#include <QToolBar>

static const QByteArray s_x11AppMenuServiceNamePropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_SERVICE_NAME");
static const QByteArray s_x11AppMenuObjectPathPropertyName = QByteArrayLiteral("_KDE_NET_WM_APPMENU_OBJECT_PATH");
static QHash<QByteArray, xcb_atom_t> s_atoms;

AppMenuWidget::AppMenuWidget(QWidget *parent)
    : QWidget(parent),
    m_appMenuDBus(new AppmenuDBus(this)),
    m_serviceWatcher(new QDBusServiceWatcher(this))
{
    QHBoxLayout *layout = new QHBoxLayout;
    setLayout(layout);

    m_titleLabel = new QLabel;
    m_bar = new QToolBar(this);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_bar);

    reconfigure();

    m_appMenuDBus->connectToBus();

    connect(m_appMenuDBus, &AppmenuDBus::appShowMenu, this, [=]  (int x, int y, const QString &serviceName, const QDBusObjectPath &menuObjectPath, int actionId){
        qDebug() << x << y << serviceName << "appShowMenu";
    });
}

void AppMenuWidget::reconfigure()
{
    // Setup a menu importer if needed
    if (!m_menuImporter) {
        QDBusConnection::sessionBus().connect({}, {}, QStringLiteral("com.canonical.dbusmenu"),
                                                                QStringLiteral("ItemActivationRequested"),
                                                        this, SLOT(itemActivationRequested(int,uint)));

        m_menuImporter = new MenuImporter(this);
        connect(m_menuImporter, &MenuImporter::WindowRegistered, this, &AppMenuWidget::slotWindowRegistered);
        m_menuImporter->connectToBus();
    }

    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this, &AppMenuWidget::onActiveWindowChanged);
}

void AppMenuWidget::onActiveWindowChanged(WId id)
{
    auto *c = QX11Info::connection();
    auto getWindowPropertyString = [c](WId id, const QByteArray &name) -> QByteArray {
        QByteArray value;
        if (!s_atoms.contains(name)) {
            const xcb_intern_atom_cookie_t atomCookie = xcb_intern_atom(c, false, name.length(), name.constData());
            QScopedPointer<xcb_intern_atom_reply_t, QScopedPointerPodDeleter> atomReply(xcb_intern_atom_reply(c, atomCookie, nullptr));
            if (atomReply.isNull()) {
                return value;
            }

            s_atoms[name] = atomReply->atom;
            if (s_atoms[name] == XCB_ATOM_NONE) {
                 return value;
            }
        }

        static const long MAX_PROP_SIZE = 10000;
        auto propertyCookie = xcb_get_property(c, false, id, s_atoms[name], XCB_ATOM_STRING, 0, MAX_PROP_SIZE);
        QScopedPointer<xcb_get_property_reply_t, QScopedPointerPodDeleter> propertyReply(xcb_get_property_reply(c, propertyCookie, nullptr));
        if (propertyReply.isNull()) {
            return value;
        }

        if (propertyReply->type == XCB_ATOM_STRING && propertyReply->format == 8 && propertyReply->value_len > 0) {
            const char *data = (const char *) xcb_get_property_value(propertyReply.data());
            int len = propertyReply->value_len;
            if (data) {
                value = QByteArray(data, data[len - 1] ? len : len - 1);
            }
        }

        return value;
    };

    auto updateMenuFromWindowIfHasMenu = [this, &getWindowPropertyString](WId id) {
        const QString serviceName = QString::fromUtf8(getWindowPropertyString(id, s_x11AppMenuServiceNamePropertyName));
        const QString menuObjectPath = QString::fromUtf8(getWindowPropertyString(id, s_x11AppMenuObjectPathPropertyName));

        if (!serviceName.isEmpty() && !menuObjectPath.isEmpty()) {
            updateApplicationMenu(serviceName, menuObjectPath);
            return true;
        }
        return false;
    };

    m_bar->clear();
    m_titleLabel->setText("");

    KWindowInfo info(id, NET::WMState | NET::WMWindowType, NET::WM2TransientFor | NET::WM2WindowClass);
    if (info.hasState(NET::SkipTaskbar) ||
            info.windowType(NET::UtilityMask) == NET::Utility ||
            info.windowType(NET::DesktopMask) == NET::Desktop) {
        return;
    }

    m_titleLabel->setText(info.windowClassClass());

    WId transientId = info.transientFor();
    // lok at transient windows first
    while (transientId) {
        if (updateMenuFromWindowIfHasMenu(transientId)) {
            // setVisible(true);
            return;
        }
        transientId = KWindowInfo(transientId, nullptr, NET::WM2TransientFor).transientFor();
    }

    if (updateMenuFromWindowIfHasMenu(id)) {
        // setVisible(true);
        return;
    }
}

void AppMenuWidget::itemActivationRequested(int actionId, uint timeStamp)
{
    // qDebug() << message().service() << message().path() << " !!!";
}

void AppMenuWidget::slotWindowRegistered(WId id, const QString &serviceName, const QDBusObjectPath &menuObjectPath)
{
    // unless
}

void AppMenuWidget::updateApplicationMenu(const QString &serviceName, const QString &menuObjectPath)
{
    if (m_serviceName == serviceName && m_menuObjectPath == menuObjectPath) {
        if (m_importer) {
            QMetaObject::invokeMethod(m_importer, "updateMenu", Qt::QueuedConnection);
        }
        return;
    }

    m_serviceName = serviceName;
    m_menuObjectPath = menuObjectPath;
    m_serviceWatcher->setWatchedServices(QStringList({m_serviceName}));

    if (m_importer) {
        m_importer->deleteLater();
    }

    qDebug() << "updateApplicationMenu():" << m_serviceName << m_menuObjectPath;

    m_importer = new QDBusMenuImporter(serviceName, menuObjectPath, this);
    QMetaObject::invokeMethod(m_importer, "updateMenu", Qt::QueuedConnection);

    connect(m_importer.data(), &DBusMenuImporter::menuUpdated, this, [=] {
        QMenu *menu = m_importer->menu();

        if (menu) {
            menu->setParent(this);

            for (QAction *a : menu->actions()) {
                qDebug() << a->text();

                m_bar->addAction(a);
            }
        }
    });
}

bool AppMenuWidget::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);

    if (!KWindowSystem::isPlatformX11() || eventType != "xcb_generic_event_t") {
        return false;
    }

    auto e = static_cast<xcb_generic_event_t *>(message);
    const uint8_t type = e->response_type & ~0x80;
    if (type == XCB_PROPERTY_NOTIFY) {
        auto *event = reinterpret_cast<xcb_property_notify_event_t *>(e);
        if (event->window == m_delayedMenuWindowId) {
            auto serviceNameAtom = s_atoms.value(s_x11AppMenuServiceNamePropertyName);
            auto objectPathAtom = s_atoms.value(s_x11AppMenuObjectPathPropertyName);

            if (serviceNameAtom != XCB_ATOM_NONE && objectPathAtom != XCB_ATOM_NONE) { // shouldn't happen
                if (event->atom == serviceNameAtom || event->atom == objectPathAtom) {
                    // see if we now have a menu
                    // onActiveWindowChanged(KWindowSystem::activeWindow());

                    qDebug() << "fuck ooff ?? ";
                }
            }
        }
    }

    return false;
}
