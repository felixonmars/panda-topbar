#ifndef MENUWIDGET__H
#define MENUWIDGET__H

#include <QToolButton>
#include <QGraphicsWidget>
#include <QTimer>

class QGraphicsLinearLayout;
class QGraphicsView;

class MenuWidget : public QGraphicsWidget
{
Q_OBJECT
public:
    MenuWidget(QGraphicsView *view = 0);
    ~MenuWidget();

    /**
     * Set root menu
     */
    void setMenu(QMenu *menu);
    /**
     *  Init layout with root menu
     */
    void initLayout();
    /**
     * True if a menu is visible in menuwidget
     */
    bool aMenuIsVisible() { return m_visibleMenu; }
    /**
     * Activate action, or first action if null
     */
    void setActiveAction(QAction *action);

    /**
     * Auto open menu items on mouse over
     */
    void autoOpen() { m_mouseTimer->start(100); }

    /**
     * Return content bottom margin
     */
    qreal contentBottomMargin() { return m_contentBottomMargin; }

    void hide();

protected:
    /**
     * Use to get keyboard events
     */
    virtual bool eventFilter(QObject*, QEvent*);
    /**
     * Filter events on main menu
     */
    bool menuEventFilter(QEvent* event);
    /**
     * Filter events on submenus
     */
    bool subMenuEventFilter(QObject* object, QEvent* event);
private Q_SLOTS:
    /**
     * Clean menu if destroyed
     */
    void slotMenuDestroyed();
    /**
     * Check hovered item and active it
     */
    void slotCheckActiveItem();
    /**
     * A menu is hidding
     */
    void slotMenuAboutToHide();
    /**
     * Menubar button clicked
     */
    void slotButtonClicked();
    /**
     * Update pending actions
     */
    void slotUpdateActions();
Q_SIGNALS:
    void needResize();
    void aboutToHide();
private:
    /**
     * Return a button based on action
     */
    QToolButton* createButton(QAction *action);
    /**
     * Show current button menu
     * return showed menu
     */
    QMenu* showMenu();
    /**
     * Show next menu if next, otherwise previous
     */
    void showLeftRightMenu(bool next);
    /**
     * Install event filter for menu and it submenus
     */
    void installEventFilterForAll(QMenu *menu, QObject *object);

    //Follow mouse position
    QTimer *m_mouseTimer;
    //Update actions
    QTimer *m_actionTimer;
    QGraphicsView *m_view;
    QGraphicsLinearLayout *m_layout;
    QList<QToolButton*> m_buttons;
    QToolButton *m_currentButton;
    qreal m_contentBottomMargin;
    QPoint m_mousePosition;
    QMenu *m_visibleMenu;
    QMenu *m_menu;
};

#endif //MENUWIDGET__H