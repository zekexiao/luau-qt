#ifndef TEST_H
#define TEST_H

#include <QObject>

class Test : public QObject
{
    Q_OBJECT

public:
    Test(QObject *parent = nullptr)
        : QObject(parent)
    {}

private Q_SLOTS:
    void testBool();
    void testInt();
    void testDouble();
    void testString();
    void testList();
    void testMap();
    void testVector();
    void testPoint();
};
#endif // TEST_H
