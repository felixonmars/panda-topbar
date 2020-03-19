#include "mainwindow.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QScreen>
#include <QPainter>

#include <KF5/KWindowSystem/KWindowSystem>
#include <KF5/KWindowSystem/KWindowEffects>

#define TOPBAR_HEIGHT 35

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent),
      m_fakeWidget(new QWidget(nullptr)),
      m_mainPanel(new MainPanel)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addSpacing(10);
    layout->addWidget(m_mainPanel);
    layout->addSpacing(10);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    m_fakeWidget->setFocusPolicy(Qt::NoFocus);
    m_fakeWidget->setWindowFlags(Qt::FramelessWindowHint);
    m_fakeWidget->setAttribute(Qt::WA_TranslucentBackground);

    setAttribute(Qt::WA_NoSystemBackground, false);
    setAttribute(Qt::WA_TranslucentBackground);
    // setWindowFlags(Qt::X11BypassWindowManagerHint);

    KWindowSystem::setOnDesktop(effectiveWinId(), NET::OnAllDesktops);
    KWindowSystem::setType(winId(), NET::Dock);
    KWindowEffects::enableBlurBehind(winId(), true);

    initSize();

    connect(qApp->primaryScreen(), &QScreen::geometryChanged, this, &MainWindow::initSize);
}

MainWindow::~MainWindow()
{

}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QColor color("#FFFFFF");
    color.setAlpha(20);
    painter.fillRect(rect(), color);
}

void MainWindow::initSize()
{
    QRect primaryRect = qApp->primaryScreen()->geometry();
    qreal scale = qApp->primaryScreen()->devicePixelRatio();
    setFixedWidth(primaryRect.width());
    setFixedHeight(TOPBAR_HEIGHT);
    move(0, 0);

    setStrutPartial();

    KWindowSystem::setState(winId(), NET::SkipTaskbar);
    KWindowSystem::setState(winId(), NET::SkipSwitcher);
}

void MainWindow::setStrutPartial()
{
    // 不清真的作法，kwin设置blur后设置程序支撑导致模糊无效
    QRect r(geometry());
    r.setHeight(1);
    r.setWidth(1);
    m_fakeWidget->setGeometry(r);
    m_fakeWidget->setVisible(true);
    KWindowSystem::setState(m_fakeWidget->winId(), NET::SkipTaskbar);
    KWindowSystem::setState(m_fakeWidget->winId(), NET::SkipSwitcher);

    const QRect windowRect = this->rect();
    NETExtendedStrut strut;

    strut.top_width = height();
    strut.top_start = x();
    strut.top_end = x() + width();

    KWindowSystem::setExtendedStrut(m_fakeWidget->winId(),
                                     strut.left_width,
                                     strut.left_start,
                                     strut.left_end,
                                     strut.right_width,
                                     strut.right_start,
                                     strut.right_end,
                                     strut.top_width,
                                     strut.top_start,
                                     strut.top_end,
                                     strut.bottom_width,
                                     strut.bottom_start,
                                     strut.bottom_end);
}
