#ifndef QLUA_H
#define QLUA_H

#include "qluaglobal.h"
#include <QObject>
#include <QVariant>

class QLuaPrivate;

class QLUA_EXPORT QLua : public QObject
{
    Q_OBJECT

public:
    explicit QLua(QObject *parent = nullptr);

    ~QLua() override;

    void load(const QString &code);

    QVariant call(const QString &funcName, const QVariantList &args);

private:
    QLuaPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QLua)
};

#endif // QLUA_H
