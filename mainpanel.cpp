#include "mainpanel.h"
#include "datetimewidget.h"
#include <QHBoxLayout>

MainPanel::MainPanel(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addStretch();
    layout->addWidget(new DateTimeWidget, 0, Qt::AlignVCenter);
    setLayout(layout);
}
