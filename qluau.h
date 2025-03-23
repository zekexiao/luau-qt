#ifndef QLUAU_H
#define QLUAU_H

#include <QObject>
#include <QVariant>

class QLuauPrivate;
class QLuau : public QObject
{
    Q_OBJECT
public:
    explicit QLuau(QObject *parent = nullptr);
    ~QLuau() override;
    void load(const QString &code);
    QVariantList call(const QString &funcName, const QVariantList &args);

private:
    QLuauPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QLuau)
};

#endif // QLUAU_H
