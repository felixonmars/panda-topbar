#include "statusnotifier.h"
#include <QDebug>

Statusnotifier::Statusnotifier(QObject *parent)
  : QObject(parent)
{
    qDebug() << " hello world!";
}