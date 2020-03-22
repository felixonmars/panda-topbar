#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFrame>
#include "mainpanel.h"

class MainWindow : public QFrame
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    void initSize();
    void setStrutPartial();

private:
    QWidget *m_fakeWidget;
    MainPanel *m_mainPanel;
};

#endif // MAINWINDOW_H
