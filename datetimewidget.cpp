#include "datetimewidget.h"
#include <QHBoxLayout>
#include <QDateTime>

DateTimeWidget::DateTimeWidget(QWidget *parent)
    : QWidget(parent),
      m_label(new QLabel),
      m_refreshTimer(new QTimer(this))
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_label);
    setLayout(layout);

    updateCurrentTimeString();

    m_refreshTimer->setInterval(1000);
    m_refreshTimer->start();
    connect(m_refreshTimer, &QTimer::timeout, this, &DateTimeWidget::updateCurrentTimeString);
}

void DateTimeWidget::updateCurrentTimeString()
{
    const QDateTime currentDateTime = QDateTime::currentDateTime();

    m_label->setText(currentDateTime.toString("HH:mm"));
}
