#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
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
};

#endif // MAINWINDOW_H
