#ifndef STATUSNOTIFIERWIDGET_H
#define STATUSNOTIFIERWIDGET_H

#include <QDir>
#include <QHBoxLayout>
#include "statusnotifierbutton.h"
#include "statusnotifierwatcher.h"

class StatusNotifierWidget : public QWidget
{
    Q_OBJECT

public:
    StatusNotifierWidget(QWidget *parent = nullptr);
    ~StatusNotifierWidget();

public slots:
    void itemAdded(QString serviceAndPath);
    void itemRemoved(const QString &serviceAndPath);

private:
    QHBoxLayout *m_layout;
    StatusNotifierWatcher *mWatcher;

    QHash<QString, StatusNotifierButton*> mServices;
};

#endif // STATUSNOTIFIERWIDGET_H