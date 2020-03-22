#ifndef KDBUSIMPORTER_H
#define KDBUSIMPORTER_H

#include <dbusmenuimporter.h>

#include <QIcon>

class KDBusMenuImporter : public DBusMenuImporter
{

public:
    KDBusMenuImporter(const QString &service, const QString &path, QObject *parent)
        : DBusMenuImporter(service, path, parent)
    {

    }

protected:
    QIcon iconForName(const QString &name) override {
        return QIcon::fromTheme(name);
    }

    QMenu *createMenu(QWidget *parent) override {
        //return new VerticalMenu(parent);
        return nullptr;
    }
};

#endif //KDBUSIMPORTER_H
