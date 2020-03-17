#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

class DateTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeWidget(QWidget *parent = nullptr);

private:
    void updateCurrentTimeString();

private:
    QLabel *m_label;
    QTimer *m_refreshTimer;
};

#endif // DATETIMEWIDGET_H
