#include <QDebug>
#include <QApplication>
#include <QResizeEvent>
#include <QPainter>
#include <QBitmap>
#include <QStyle>
#include <QScreen>
#include <QTimer>

#include "trayicon.h"
#include "xfitman.h"

#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>

#define XEMBED_EMBEDDED_NOTIFY 0

static bool xError;

int windowErrorHandler(Display *d, XErrorEvent *e)
{
    xError = true;

    if (e->error_code != BadWindow) {
        char str[1024];
        XGetErrorText(d, e->error_code,  str, 1024);
        qWarning() << "Error handler" << e->error_code
                   << str;
    }

    return 0;
}


TrayIcon::TrayIcon(Window id, QWidget *parent)
    : QWidget(parent),
      m_iconId(id),
      m_windowId(0),
      m_appName(xfitMan().getApplicationName(m_iconId)),
      m_iconSize(QSize(24, 24)),
      m_display(QX11Info::display())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // NOTE:
    // see https://github.com/lxqt/lxqt/issues/945
    // workaround: delayed init because of weird behaviour of some icons/windows (claws-mail)
    // (upon starting the app the window for receiving clicks wasn't correctly sized
    //  no matter what we've done)
    QTimer::singleShot(200, [this] { init(); update(); } );
}

TrayIcon::~TrayIcon()
{
    Display* dsp = m_display;
    XSelectInput(dsp, m_iconId, NoEventMask);

    if (m_damage)
        XDamageDestroy(dsp, m_damage);

    // reparent to root
    xError = false;
    XErrorHandler old = XSetErrorHandler(windowErrorHandler);

    XUnmapWindow(dsp, m_iconId);
    XReparentWindow(dsp, m_iconId, QX11Info::appRootWindow(), 0, 0);

    if (m_windowId)
        XDestroyWindow(dsp, m_windowId);
    XSync(dsp, False);
    XSetErrorHandler(old);
}

void TrayIcon::setIconSize(QSize iconSize)
{
    m_iconSize = iconSize;

    const QSize req_size{m_iconSize * metric(PdmDevicePixelRatio)};
    if (m_windowId)
        xfitMan().resizeWindow(m_windowId, req_size.width(), req_size.height());

    if (m_iconId)
        xfitMan().resizeWindow(m_iconId, req_size.width(), req_size.height());
}

QSize TrayIcon::sizeHint() const
{
    QMargins margins = contentsMargins();
    return QSize(margins.left() + m_iconSize.width() + margins.right(),
                 margins.top() + m_iconSize.height() + margins.bottom());
}

void TrayIcon::windowDestroyed(Window w)
{
    //damage is destroyed if it's parent window was destroyed
    if (m_iconId == w)
        m_damage = 0;
}

bool TrayIcon::event(QEvent *event)
{
    if (m_windowId) {
        switch (event->type()) {
        case QEvent::Paint:
            draw(static_cast<QPaintEvent *>(event));
            break;

        case QEvent::Move:
        case QEvent::Resize: {
            QRect rect = iconGeometry();
            xfitMan().moveWindow(m_windowId, rect.left(), rect.top());
        }
        break;

        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
            event->accept();
            break;

        default:
            break;
        }
    }

    return QWidget::event(event);
}

void TrayIcon::draw(QPaintEvent *event)
{
    Display *dsp = m_display;
    XWindowAttributes attr;
    if (!XGetWindowAttributes(dsp, m_iconId, &attr)) {
        qWarning() << "Paint error";
        return;
    }

    QImage image;
    XImage* ximage = XGetImage(dsp, m_iconId, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);
    if (ximage) {
        image = QImage((const uchar*) ximage->data, ximage->width, ximage->height, ximage->bytes_per_line,  QImage::Format_ARGB32_Premultiplied);
    } else {
        qWarning() << "    * Error image is NULL";

        XClearArea(m_display, (Window)winId(), 0, 0, attr.width, attr.height, False);
        // for some unknown reason, XGetImage failed. try another less efficient method.
        // QScreen::grabWindow uses XCopyArea() internally.
        image = qApp->primaryScreen()->grabWindow(m_iconId).toImage();
    }

    QPainter painter(this);
    QRect iconRect = iconGeometry();
    if (image.size() != iconRect.size()) {
        image = image.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QRect r = image.rect();
        r.moveCenter(iconRect.center());
        iconRect = r;
    }

    painter.drawImage(iconRect, image);

    if (ximage)
        XDestroyImage(ximage);
}

void TrayIcon::init()
{
    Display *dsp = m_display;

    XWindowAttributes attr;
    if (!XGetWindowAttributes(dsp, m_iconId, &attr)) {
        deleteLater();
        return;
    }

    unsigned long mask = 0;
    XSetWindowAttributes set_attr;

    Visual *visual = attr.visual;
    set_attr.colormap = attr.colormap;
    set_attr.background_pixel = 0;
    set_attr.border_pixel = 0;
    mask = CWColormap|CWBackPixel|CWBorderPixel;

    const QRect icon_geom = iconGeometry();
    m_windowId = XCreateWindow(dsp, this->winId(), icon_geom.x(),
                               icon_geom.y(), icon_geom.width() * metric(PdmDevicePixelRatio),
                               icon_geom.height() * metric(PdmDevicePixelRatio),
                               0, attr.depth, InputOutput, visual, mask, &set_attr);


    xError = false;
    XErrorHandler old;
    old = XSetErrorHandler(windowErrorHandler);
    XReparentWindow(dsp, m_iconId, m_windowId, 0, 0);
    XSync(dsp, false);
    XSetErrorHandler(old);

    if (xError) {
        qWarning() << "****************************************";
        qWarning() << "* Not icon_swallow                     *";
        qWarning() << "****************************************";
        XDestroyWindow(dsp, m_windowId);
        m_windowId = 0;
        deleteLater();
        return;
    }

    {
        Atom acttype;
        int actfmt;
        unsigned long nbitem, bytes;
        unsigned char *data = 0;
        int ret;

        ret = XGetWindowProperty(dsp, m_iconId, xfitMan().atom("_XEMBED_INFO"),
                                 0, 2, false, xfitMan().atom("_XEMBED_INFO"),
                                 &acttype, &actfmt, &nbitem, &bytes, &data);
        if (ret == Success) {
            if (data)
                XFree(data);
        } else {
            qWarning() << "TrayIcon: xembed error";
            XDestroyWindow(dsp, m_windowId);
            deleteLater();
            return;
        }
    }

    {
        XEvent e;
        e.xclient.type = ClientMessage;
        e.xclient.serial = 0;
        e.xclient.send_event = True;
        e.xclient.message_type = xfitMan().atom("_XEMBED");
        e.xclient.window = m_iconId;
        e.xclient.format = 32;
        e.xclient.data.l[0] = CurrentTime;
        e.xclient.data.l[1] = XEMBED_EMBEDDED_NOTIFY;
        e.xclient.data.l[2] = 0;
        e.xclient.data.l[3] = m_windowId;
        e.xclient.data.l[4] = 0;
        XSendEvent(dsp, m_iconId, false, 0xFFFFFF, &e);
    }

    XSelectInput(dsp, m_iconId, StructureNotifyMask);
    m_damage = XDamageCreate(dsp, m_iconId, XDamageReportRawRectangles);
    XCompositeRedirectWindow(dsp, m_windowId, CompositeRedirectManual);

    XMapWindow(dsp, m_iconId);
    XMapRaised(dsp, m_windowId);

    const QSize req_size { m_iconSize * metric(PdmDevicePixelRatio) };
    XResizeWindow(dsp, m_iconId, req_size.width(), req_size.height());
}

QRect TrayIcon::iconGeometry()
{
    QRect res = QRect(QPoint(0, 0), m_iconSize);
    res.moveCenter(QRect(0, 0, width(), height()).center());

    return res;
}
