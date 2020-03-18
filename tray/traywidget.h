#ifndef TRAYWIDGET_H
#define TRAYWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QAbstractNativeEventFilter>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <xcb/xcb_event.h>
#include "fixx11h.h"

class TrayIcon;
class QSize;

class TrayWidget : public QWidget, QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit TrayWidget(QWidget *parent = nullptr);
    ~TrayWidget();

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);

private:
    void startTray();
    void stopTray();

    VisualID getVisual();
    void clientMessageEvent(xcb_generic_event_t *e);
    TrayIcon *findIcon(Window id);
    void setIconSize(QSize iconSize);

    void addIcon(Window id);
    void sortIcons();

    void onIconDestroyed(QObject *icon);

private:
    bool m_isValid;
    Window m_trayId;
    QList<TrayIcon *> m_icons;
    int m_damageEvent;
    int m_damageError;
    QSize m_iconSize;
    Atom _NET_SYSTEM_TRAY_OPCODE;
    Display *m_display;

    QHBoxLayout *m_layout;
};

#endif // TRAYWIDGET_H
