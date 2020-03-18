#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QX11Info>
#include <algorithm>
#include <vector>
#include "trayicon.h"
#include "traywidget.h"
#include "xfitman.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/Xdamage.h>
#include <xcb/xcb.h>
#include <xcb/damage.h>

#undef Bool // defined as int in X11/Xlib.h

#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0
#define _NET_SYSTEM_TRAY_ORIENTATION_VERT 1

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define XEMBED_EMBEDDED_NOTIFY  0
#define XEMBED_MAPPED          (1 << 0)

TrayWidget::TrayWidget(QWidget *parent)
    : QWidget(parent),
      m_isValid(false),
      m_trayId(0),
      m_damageEvent(0),
      m_damageError(0),
      m_iconSize(QSize(32, 32)),
      m_display(QX11Info::display())
{
    m_layout = new QHBoxLayout;
    _NET_SYSTEM_TRAY_OPCODE = XfitMan::atom("_NET_SYSTEM_TRAY_OPCODE");
    setLayout(m_layout);

    // Init the selection later just to ensure that no signals are sent until
    // after construction is done and the creating object has a chance to connect.
    QTimer::singleShot(0, this, &TrayWidget::startTray);
}

TrayWidget::~TrayWidget()
{
    stopTray();
}

bool TrayWidget::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType != "xcb_generic_event_t")
        return false;

    xcb_generic_event_t *event = static_cast<xcb_generic_event_t *>(message);
    TrayIcon *icon;
    int event_type = event->response_type & ~0x80;

    switch (event_type)
    {
        case ClientMessage:
            clientMessageEvent(event);
            break;

//        case ConfigureNotify:
//            icon = findIcon(event->xconfigure.window);
//            if (icon)
//                icon->configureEvent(&(event->xconfigure));
//            break;

        case DestroyNotify: {
            unsigned long event_window;
            event_window = reinterpret_cast<xcb_destroy_notify_event_t*>(event)->window;
            icon = findIcon(event_window);
            if (icon) {
                icon->windowDestroyed(event_window);
                m_icons.removeAll(icon);
                delete icon;
            }
            break;
        }
        default:
            if (event_type == m_damageEvent + XDamageNotify) {
                xcb_damage_notify_event_t* dmg = reinterpret_cast<xcb_damage_notify_event_t*>(event);
                icon = findIcon(dmg->drawable);
                if (icon)
                    icon->update();
            }
            break;
    }

    return false;
}

void TrayWidget::startTray()
{
    Display *dsp = m_display;
    Window root = QX11Info::appRootWindow();

    QString s = QStringLiteral("_NET_SYSTEM_TRAY_S%1").arg(DefaultScreen(dsp));
    Atom _NET_SYSTEM_TRAY_S = XfitMan::atom(s.toLatin1().constData());

    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != None) {
        qWarning() << "Another systray is running";
        m_isValid = false;
        return;
    }

    // init systray protocol
    m_trayId = XCreateSimpleWindow(dsp, root, -1, -1, 1, 1, 0, 0, 0);

    XSetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S, m_trayId, CurrentTime);
    if (XGetSelectionOwner(dsp, _NET_SYSTEM_TRAY_S) != m_trayId) {
        qWarning() << "Can't get systray manager";
        stopTray();
        m_isValid = false;
        return;
    }

    int orientation = _NET_SYSTEM_TRAY_ORIENTATION_HORZ;
    XChangeProperty(dsp,
                    m_trayId,
                    XfitMan::atom("_NET_SYSTEM_TRAY_ORIENTATION"),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char *) &orientation,
                    1);

    VisualID visualId = getVisual();
    if (visualId) {
        XChangeProperty(m_display,
                        m_trayId,
                        XfitMan::atom("_NET_SYSTEM_TRAY_VISUAL"),
                        XA_VISUALID,
                        32,
                        PropModeReplace,
                        (unsigned char*)&visualId,
                        1);
    }

    setIconSize(m_iconSize);

    XClientMessageEvent ev;
    ev.type = ClientMessage;
    ev.window = root;
    ev.message_type = XfitMan::atom("MANAGER");
    ev.format = 32;
    ev.data.l[0] = CurrentTime;
    ev.data.l[1] = _NET_SYSTEM_TRAY_S;
    ev.data.l[2] = m_trayId;
    ev.data.l[3] = 0;
    ev.data.l[4] = 0;
    XSendEvent(dsp, root, False, StructureNotifyMask, (XEvent*)&ev);

    XDamageQueryExtension(m_display, &m_damageEvent, &m_damageError);

    qDebug() << "Systray started";
    m_isValid = true;

    qApp->installNativeEventFilter(this);
}

void TrayWidget::stopTray()
{
    for (auto & icon : m_icons)
        disconnect(icon, &QObject::destroyed, this, &TrayWidget::onIconDestroyed);

    qDeleteAll(m_icons);

    if (m_trayId) {
        XDestroyWindow(m_display, m_trayId);
        m_trayId = 0;
    }

    m_isValid = false;
}

VisualID TrayWidget::getVisual()
{
    VisualID visualId = 0;
    Display* dsp = m_display;

    XVisualInfo templ;
    templ.screen = QX11Info::appScreen();
    templ.depth = 32;
    templ.c_class = TrueColor;

    int nvi;
    XVisualInfo* xvi = XGetVisualInfo(dsp, VisualScreenMask|VisualDepthMask|VisualClassMask, &templ, &nvi);

    if (xvi) {
        int i;
        XRenderPictFormat* format;
        for (i = 0; i < nvi; i++) {
            format = XRenderFindVisualFormat(dsp, xvi[i].visual);
            if (format &&
                format->type == PictTypeDirect &&
                format->direct.alphaMask) {
                visualId = xvi[i].visualid;
                break;
            }
        }
        XFree(xvi);
    }

    return visualId;
}

void TrayWidget::clientMessageEvent(xcb_generic_event_t *e)
{
    unsigned long opcode;
    unsigned long message_type;
    Window id;
    xcb_client_message_event_t* event = reinterpret_cast<xcb_client_message_event_t*>(e);
    uint32_t* data32 = event->data.data32;
    message_type = event->type;
    opcode = data32[1];

    if (message_type != _NET_SYSTEM_TRAY_OPCODE)
        return;

    switch (opcode) {
        case SYSTEM_TRAY_REQUEST_DOCK:
            id = data32[2];
            if (id)
                addIcon(id);
            break;


        case SYSTEM_TRAY_BEGIN_MESSAGE:
        case SYSTEM_TRAY_CANCEL_MESSAGE:
            qDebug() << "we don't show balloon messages.";
            break;


        default:
//            if (opcode == xfitMan().atom("_NET_SYSTEM_TRAY_MESSAGE_DATA"))
//                qDebug() << "message from dockapp:" << e->data.b;
//            else
//                qDebug() << "SYSTEM_TRAY : unknown message type" << opcode;
            break;
    }
}

TrayIcon *TrayWidget::findIcon(Window id)
{
    for (TrayIcon* icon : qAsConst(m_icons)) {
        if (icon->iconId() == id || icon->windowId() == id)
            return icon;
    }

    return 0;
}

void TrayWidget::setIconSize(QSize iconSize)
{
    m_iconSize = iconSize;
    unsigned long size = qMin(m_iconSize.width(), m_iconSize.height());
    XChangeProperty(m_display,
                    m_trayId,
                    XfitMan::atom("_NET_SYSTEM_TRAY_ICON_SIZE"),
                    XA_CARDINAL,
                    32,
                    PropModeReplace,
                    (unsigned char*)&size,
                    1);
}

void TrayWidget::addIcon(Window id)
{
    // decline to add an icon for a window we already manage
    TrayIcon *icon = findIcon(id);
    if (icon)
        return;

    icon = new TrayIcon(id, this);
    m_icons.append(icon);
    m_layout->addWidget(icon, 0, Qt::AlignRight);
    connect(icon, &QObject::destroyed, this, &TrayWidget::onIconDestroyed);
    sortIcons();

    qDebug() << "add Icon: " << id << icon->appName();
}

void TrayWidget::sortIcons()
{
    std::vector<QLayoutItem *> items;

    // temporarily remove all icons to sort them
    for(QLayoutItem *item; (item = m_layout->takeAt(0)) != nullptr;)
        items.push_back(item);

    std::stable_sort(items.begin(), items.end(), [](QLayoutItem *a, QLayoutItem *b) {
        auto ai = static_cast<TrayIcon *>(a->widget());
        auto bi = static_cast<TrayIcon *>(b->widget());
        return (ai->appName() < bi->appName());
    });

    // add them back in sorted order
    for(QLayoutItem *item : items)
        m_layout->addItem(item);
}

void TrayWidget::onIconDestroyed(QObject *icon)
{
    //in the time QOjbect::destroyed is emitted, the child destructor
    //is already finished, so the qobject_cast to child will return nullptr in all cases
    m_icons.removeAll(static_cast<TrayIcon *>(icon));
}
