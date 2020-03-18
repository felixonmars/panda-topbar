#ifndef TRAYICON_H
#define TRAYICON_H

#include <QWidget>

#include <X11/X.h>
#include <X11/extensions/Xdamage.h>

class QWidget;

class TrayIcon : public QWidget
{
    Q_OBJECT
public:
    explicit TrayIcon(Window id, QWidget *parent = nullptr);
    virtual ~TrayIcon();

    Window iconId() const { return m_iconId; };
    Window windowId() const { return m_windowId; };
    QString appName() const { return m_appName; };

    void setIconSize(QSize iconSize);

    QSize iconSize() const { return m_iconSize; };
    QSize sizeHint() const;

    void windowDestroyed(Window w);

protected:
    bool event(QEvent *event);
    void draw(QPaintEvent* event);

private:
    void init();
    QRect iconGeometry();

private:
    Window m_iconId;
    Window m_windowId;
    QString m_appName;
    QSize m_iconSize;
    Damage m_damage;
    Display *m_display;
};

#endif // TRAYICON_H
