#include "test.h"

#include "qlua.h"
#include <QDebug>
#include <QTest>
#include <QVector3D>

void Test::testBool()
{
    QString testCode(R"(
        function test_bool()
            return true
        end
    )");
    QLua qluau;
    qluau.load(testCode);
    QVariant result = qluau.call("test_bool", {});
    QVERIFY(result.isValid());
    QCOMPARE(result.toBool(), true);
}

void Test::testInt()
{
    QString testCode(R"(
        function test_int()
            return 42
        end
    )");
    QLua qluau;
    qluau.load(testCode);
    QVariant result = qluau.call("test_int", {});
    QVERIFY(result.isValid());
    QCOMPARE(result.toInt(), 42);
}

void Test::testDouble()
{
    QString testCode(R"(
        function test_double()
            return 3.14
        end
    )");
    QLua qluau;
    qluau.load(testCode);
    QVariant result = qluau.call("test_double", {});
    QVERIFY(result.isValid());
    QCOMPARE(result.toDouble(), 3.14);
}

void Test::testString()
{
    QString testCode(R"(
        function test_string()
            return "hello from lua"
        end
    )");
    QLua qluau;
    qluau.load(testCode);
    QVariant result = qluau.call("test_string", {});
    QVERIFY(result.isValid());
    QCOMPARE(result.toString(), QString("hello from lua"));
}

void Test::testList()
{
    QString testCode(R"(
        function test_list()
            return {1, 2, 3, 4, 5}
        end
    )");
    QLua qluau;
    qluau.load(testCode);
    QVariant result = qluau.call("test_list", {});
    QVERIFY(result.isValid());
    QList<QVariant> list = result.toList();
    QCOMPARE(list.size(), 5);
    QCOMPARE(list.at(0).toInt(), 1);
    QCOMPARE(list.at(1).toInt(), 2);
    QCOMPARE(list.at(2).toInt(), 3);
    QCOMPARE(list.at(3).toInt(), 4);
    QCOMPARE(list.at(4).toInt(), 5);
}

void Test::testMap()
{
    QString testCode(R"(
        function test_map()
            return {first=1, second=2, third=3}
        end
    )");
    QLua qluau;
    qluau.load(testCode);
    QVariant result = qluau.call("test_map", {});
    QVERIFY(result.isValid());
    auto map = result.value<QVariantMap>();
    QCOMPARE(map.size(), 3);
    QCOMPARE(map.value("first").toInt(), 1);
    QCOMPARE(map.value("second").toInt(), 2);
    QCOMPARE(map.value("third").toInt(), 3);
}

void Test::testVector()
{
    QString testCode(R"(
        function test_qvector3d(o)
            local v = vector.create(1 + o.x, 2 + o.y, 3 + o.z)
            return v
        end
    )");
    QLua qluau;
    qluau.load(testCode);
    QVector3D vector(1, 2, 3);
    QVariant result = qluau.call("test_qvector3d", {vector});
    QVERIFY(result.isValid());
    auto v = result.value<QVector3D>();
    QCOMPARE(v.x(), 2.0);
    QCOMPARE(v.y(), 4.0);
    QCOMPARE(v.z(), 6.0);
}

void Test::testPoint()
{
    QString testCode(R"(
        function test_qpoint(o)
            local p = point.create(1 + o.x, 2 + o.y)
            return p
        end
    )");
    QLua qluau;
    qluau.load(testCode);
    QPointF point(1, 2);
    QVariant result = qluau.call("test_qpoint", {point});
    QVERIFY(result.isValid());
    auto p = result.value<QPointF>();
    QCOMPARE(p.x(), 2);
    QCOMPARE(p.y(), 4);
}

QTEST_MAIN(Test)
