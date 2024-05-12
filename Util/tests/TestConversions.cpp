#include "TestConversions.h"

namespace conversions {
namespace little_endian {

void TestConversions::initTestCase() {}

void TestConversions::cleanupTestCase() {}

void TestConversions::init() {}

void TestConversions::cleanup() {}

void TestConversions::TestAsByteArrayInt() {
    QByteArray ba1 = AsByteArray(static_cast<int32_t>(343654), 4);
    QCOMPARE(ba1.size(), 4);
    QCOMPARE(ba1.at(0), static_cast<char>(0x66));
    QCOMPARE(ba1.at(1), static_cast<char>(0x3E));
    QCOMPARE(ba1.at(2), static_cast<char>(0x05));
    QCOMPARE(ba1.at(3), static_cast<char>(0x00));

    QByteArray ba2 = AsByteArray(static_cast<int32_t>(-343654), 4);
    QCOMPARE(ba2.size(), 4);
    QCOMPARE(ba2.at(0), static_cast<char>(0x9A));
    QCOMPARE(ba2.at(1), static_cast<char>(0xC1));
    QCOMPARE(ba2.at(2), static_cast<char>(0xFA));
    QCOMPARE(ba2.at(3), static_cast<char>(0xFF));
}


void TestConversions::TestAsByteArrayUInt() {
    QByteArray ba1 = AsByteArray((uint32_t)255, 1);
    QCOMPARE(ba1.size(), 1);
    QCOMPARE(ba1.at(0), static_cast<char>(0xFF));

    QByteArray ba2 = AsByteArray((uint32_t)255, 2);
    QCOMPARE(ba2.size(), 2);
    QCOMPARE(ba2.at(0), static_cast<char>(0xFF));
    QCOMPARE(ba2.at(1), static_cast<char>(0x0));

    QByteArray ba3 = AsByteArray((uint32_t)256, 2);
    QCOMPARE(ba3.size(), 2);
    QCOMPARE(ba3.at(0), static_cast<char>(0x0));
    QCOMPARE(ba3.at(1), static_cast<char>(0x1));

    QByteArray ba4 = AsByteArray((uint32_t)256, 3);
    QCOMPARE(ba4.size(), 3);
    QCOMPARE(ba4.at(0), static_cast<char>(0x0));
    QCOMPARE(ba4.at(1), static_cast<char>(0x1));
    QCOMPARE(ba4.at(2), static_cast<char>(0x0));

    QByteArray ba5 = AsByteArray((uint32_t)(256 + 255), 2);
    QCOMPARE(ba5.size(), 2);
    QCOMPARE(ba5.at(0), static_cast<char>(0xFF));
    QCOMPARE(ba5.at(1), static_cast<char>(0x1));

    try {
        AsByteArray((uint32_t)256, 1);
        QFAIL("TestAsByteArray Test should have failed");
    }
    catch(std::exception& exc) { (void)exc; }
}

void TestConversions::TestAsByteArrayFloat() {
    QByteArray ba1 = AsByteArray(1.234f);
    QCOMPARE(ba1.size(), 4);
    QCOMPARE(ba1.at(0), static_cast<char>(0xB6));
    QCOMPARE(ba1.at(1), static_cast<char>(0xF3));
    QCOMPARE(ba1.at(2), static_cast<char>(0x9D));
    QCOMPARE(ba1.at(3), static_cast<char>(0x3F));

    QByteArray ba2 = AsByteArray(0.00006774f);
    QCOMPARE(ba2.size(), 4);
    QCOMPARE(ba2.at(0), static_cast<char>(0xA3));
    QCOMPARE(ba2.at(1), static_cast<char>(0x0F));
    QCOMPARE(ba2.at(2), static_cast<char>(0x8E));
    QCOMPARE(ba2.at(3), static_cast<char>(0x38));

    QByteArray ba3 = AsByteArray((float)-9848950000);
    QCOMPARE(ba3.size(), 4);
    QCOMPARE(ba3.at(0), static_cast<char>(0xC3));
    QCOMPARE(ba3.at(1), static_cast<char>(0xC2));
    QCOMPARE(ba3.at(2), static_cast<char>(0x12));
    QCOMPARE(ba3.at(3), static_cast<char>(0xD0));
}

void TestConversions::TestAsInt32() {
    QByteArray ba1;
    ba1.resize(4);
    ba1[0] = static_cast<char>(0x66);
    ba1[1] = static_cast<char>(0x3E);
    ba1[2] = static_cast<char>(0x05);
    ba1[3] = static_cast<char>(0x00);

    QCOMPARE(AsInt32(ba1), static_cast<int32_t>(343654));

    QByteArray ba2;
    ba2.resize(4);
    ba2[0] = static_cast<char>(0x9A);
    ba2[1] = static_cast<char>(0xC1);
    ba2[2] = static_cast<char>(0xFA);
    ba2[3] = static_cast<char>(0xFF);

    QCOMPARE(AsInt32(ba2), static_cast<int32_t>(-343654));

}

void TestConversions::TestAsUInt32() {
    QByteArray ba1;
    ba1.resize(2);
    ba1[0] = static_cast<char>(0xFF);
    ba1[1] = static_cast<char>(0x1);
    QCOMPARE(AsUInt32(ba1), static_cast<uint32_t>(256 + 255));

    QByteArray ba2;
    ba2.resize(2);
    ba2[0] = static_cast<char>(0x1);
    ba2[1] = static_cast<char>(0x0);
    QCOMPARE(AsUInt32(ba2), static_cast<uint32_t>(1));

    QByteArray ba3;
    ba3.resize(3);
    ba3[0] = static_cast<char>(0x0);
    ba3[1] = static_cast<char>(0x0);
    ba3[2] = static_cast<char>(0x0);
    QCOMPARE(AsUInt32(ba3), static_cast<uint32_t>(0));

    QByteArray ba4;
    ba4.resize(4);
    ba4[0] = static_cast<char>(0xB6);
    ba4[1] = static_cast<char>(0xF3);
    ba4[2] = static_cast<char>(0x9D);
    ba4[3] = static_cast<char>(0x3F);
    QCOMPARE(AsUInt32(ba4), static_cast<uint32_t>(1067316150));

    try {
        QByteArray ba4("11111");
        AsUInt32(ba4);
        QFAIL("TestFINS::TestAsInt Test should have thrown, but didn't");
    }
    catch(std::exception& exc) { (void)exc; }
}

void TestConversions::TestAsUInt16() {
    QByteArray ba1;
    ba1.resize(2);
    ba1[0] = static_cast<char>(0xD7);
    ba1[1] = static_cast<char>(0xFF);
    QCOMPARE(AsUInt16(ba1), static_cast<uint16_t>(65495));
}

void TestConversions::TestAsUInt8() {
    QByteArray ba1;
    ba1.resize(1);
    ba1[0] = static_cast<char>(0xF3);
    QCOMPARE(AsUInt8(ba1), static_cast<uint8_t>(243));
}

void TestConversions::TestAsFloat() {
    QByteArray ba1;
    ba1.resize(4);
    ba1[0] = static_cast<char>(0xB6);
    ba1[1] = static_cast<char>(0xF3);
    ba1[2] = static_cast<char>(0x9D);
    ba1[3] = static_cast<char>(0x3F);
    QVERIFY(std::abs(AsFloat(ba1) - 1.234f) < 0.000001f);

    QByteArray ba2;
    ba2.resize(4);
    ba2[0] = static_cast<char>(0xC3);
    ba2[1] = static_cast<char>(0xC2);
    ba2[2] = static_cast<char>(0x12);
    ba2[3] = static_cast<char>(0xD0);
    QVERIFY(std::abs(AsFloat(ba2) + 9848950000.0f)/9848950000.0f < 0.000001f);
}

}
}
