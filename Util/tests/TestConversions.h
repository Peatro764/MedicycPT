#include <QtTest/QtTest>

#include "Conversions.h"

namespace conversions {
namespace little_endian {

class TestConversions : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestAsByteArrayUInt();
    void TestAsByteArrayInt();
    void TestAsByteArrayFloat();
    void TestAsInt32();
    void TestAsUInt32();
    void TestAsUInt16();
    void TestAsUInt8();
    void TestAsFloat();

private:
};

}
}
