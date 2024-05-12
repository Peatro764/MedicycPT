#include "TestSeanceConfig.h"

#include "Calc.h"

void TestSeanceConfig::initTestCase() {}

void TestSeanceConfig::cleanupTestCase() {}

void TestSeanceConfig::init() {}

void TestSeanceConfig::cleanup() {}

QByteArray TestSeanceConfig::GetByteArray() const {
    QByteArray expByteArray;
    expByteArray.resize(50);

    expByteArray[0] = 'H';
    expByteArray[1] = 'O';
    expByteArray[2] = 'F';
    expByteArray[3] = 'V';
    expByteArray[4] = 'E';
    expByteArray[5] = 'R';
    expByteArray[6] = 'B';
    expByteArray[7] = 'E';
    expByteArray[8] = 'R';
    expByteArray[9] = 'G';
    expByteArray[10] = '#';
    expByteArray[11] = ' ';
    expByteArray[12] = ' ';
    expByteArray[13] = ' ';
    expByteArray[14] = ' ';
    expByteArray[15] = ' ';
    expByteArray[16] = ' ';
    expByteArray[17] = ' ';
    expByteArray[18] = ' ';
    expByteArray[19] = ' ';

    expByteArray[20] = 0x39;
    expByteArray[21] = 0x30;
    expByteArray[22] = 0x00;
    expByteArray[23] = 0x00;

    expByteArray[24] = 0x1;
    expByteArray[25] = 0x0;

    expByteArray[26] = 0x00; // dose (13.0)
    expByteArray[27] = 0x00;
    expByteArray[28] = 0x50;
    expByteArray[29] = 0x41;

    expByteArray[30] = 0x29; // debit
    expByteArray[31] = 0x5C;
    expByteArray[32] = 0xAF;
    expByteArray[33] = 0x3F;

    expByteArray[34] = 0x98; // mu_des
    expByteArray[35] = 0x05;
    expByteArray[36] = 0x00;
    expByteArray[37] = 0x00;

    expByteArray[38] = 0x17; // mu_cor 23
    expByteArray[39] = 0x00;
    expByteArray[40] = 0x00;
    expByteArray[41] = 0x00;

    expByteArray[42] = 0xD0; // duration
    expByteArray[43] = 0x84;
    expByteArray[44] = 0x00;
    expByteArray[45] = 0x00;

    expByteArray[46] = 0xCB; // current stripper 459
    expByteArray[47] = 0x01;
    expByteArray[48] = 0x00;
    expByteArray[49] = 0x00;

    return expByteArray;
}

void TestSeanceConfig::TestByteArray() {
    SeanceConfig seance("HOFVERBERG", 12345, 1, 13.0f, 1.37f, 1432, 23, 34000, 459);

    QByteArray asByteArray = seance.ByteArray();
    QByteArray expByteArray = GetByteArray();

    QCOMPARE(asByteArray, expByteArray);
}

void TestSeanceConfig::TestUnPack() {
    QByteArray ba = GetByteArray();
    SeanceConfig seance = SeanceConfig::UnPack(ba);
    SeanceConfig expSeance("HOFVERBERG", 12345, 1, 13.0f, 1.37f, 1432, 23, 34000, 459);

    QCOMPARE(seance, expSeance);
}

void TestSeanceConfig::TestConstructor() {
    SeanceConfig s1(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    QCOMPARE(s1.name(), QString("hej"));
    QCOMPARE((int)s1.dossier(), 20000);
    QVERIFY(std::abs(s1.dose() - 13.0f) < 0.00001f);
    QVERIFY(std::abs(s1.debit() - 1.37f) < 0.00001f);
    QCOMPARE((int)s1.seance_id() , 1);
    QCOMPARE((int)s1.mu_des(), 1234);
    QCOMPARE((int)s1.mu_cor(), 23);
    QCOMPARE((int)s1.duration(), 14000);
    QCOMPARE((int)s1.current_stripper(), 459);

    try {
        SeanceConfig s1(QString("hej"), 20000, 1, 13.0f, -1.37f, 1234, 23, 14000, 459);
        QFAIL("Test should have thrown 1");
    }
    catch (std::exception& exc) {}

    try {
        SeanceConfig s1(QString("hej"), 20000, 1, -13.0f, 1.37f, 1234, 23, 14000, 459);
        QFAIL("Test should have thrown 2");
    }
    catch (std::exception& exc) {}
}

void TestSeanceConfig::TestEquality() {
    SeanceConfig s1(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s3(QString("hej"), 20001, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s4(QString("hej"), 20000, 2, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s4b(QString("hej"), 20000, 1, 13.68f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s5(QString("hej"), 20000, 1, 13.0f, 1.38f, 1234, 23, 14000, 459);
    SeanceConfig s6(QString("hej"), 20000, 1, 13.0f, 1.37f, 1235, 23, 14000, 459);
    SeanceConfig s7(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14001, 459);
    SeanceConfig s8(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 24, 14000, 459);
    SeanceConfig s9(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 460);
    QVERIFY(s1 == s1);
    QVERIFY(s1 != s3);
    QVERIFY(s1 != s4);
    QVERIFY(s1 != s4b);
    QVERIFY(s1 != s5);
    QVERIFY(s1 != s6);
    QVERIFY(s1 != s7);
    QVERIFY(s1 != s8);
    QVERIFY(s1 != s9);
}

void TestSeanceConfig::TestSetters() {
    SeanceConfig s1a(QString("hej"), 20000, 1, calc::UMToDose(1.37f, 1234), 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s1b(QString("hej"), 20000, 1, calc::UMToDose(1.37f, 1234), 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s1c(QString("hej"), 20000, 1, calc::UMToDose(1.37f, 666), 1.37f, 666, 23, 14000, 459);
    s1a.SetMUDes(666);
    QVERIFY(s1a != s1b);
    QVERIFY(s1a == s1c);


    SeanceConfig s2a(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s2b(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s2c(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 42, 14000, 459);
    s2a.SetMUCor(42);
    QVERIFY(s2a != s2b);
    QVERIFY(s2a == s2c);

    SeanceConfig s3a(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s3b(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s3c(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14100, 459);
    s3a.SetDuration(14100);
    QVERIFY(s3a != s3b);
    QVERIFY(s3a == s3c);

    SeanceConfig s4a(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s4b(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    SeanceConfig s4c(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 654);
    s4a.SetCurrentStripper(654);
    QVERIFY(s4a != s4b);
    QVERIFY(s4a == s4c);
}

void TestSeanceConfig::TestChangeUM() {
    SeanceConfig s1a(QString("hej"), 20000, 1, 13.0f, 1.37f, 1234, 23, 14000, 459);
    QVERIFY((s1a.dose() - 13.0) < 0.0001f);

    s1a.SetMUDes(100);
    QVERIFY((s1a.dose() - calc::UMToDose(1.37, 100)) < 0.0001);
}

void TestSeanceConfig::TestBelongsTo() {
    uint32_t dossier = 20000;
    uint16_t seance_id = 3;
    float dose = 13.7;
    float debit = 1.37;
    uint32_t mu_des = 455;
    uint32_t mu_cor = 0;
    uint32_t dur = 9500;
    uint32_t stripper = 400;

    SeanceConfig sc1(QString("name"), dossier, seance_id, dose, debit, mu_des, mu_cor, dur, stripper);
    SeancePacket sp_belongs(dossier, seance_id, 1, 4500, 433, 455, 500, 333, 1200, 1201);
    SeancePacket sp_notbelongs1(dossier + 1, seance_id, 1, 4500, 433, 455, 500, 333, 1200, 1201);
    SeancePacket sp_notbelongs2(dossier, seance_id + 1, 1, 4500, 433, 455, 500, 333, 1200, 1201);

    std::vector<SeancePacket> all_belongs1 { sp_belongs };
    std::vector<SeancePacket> all_belongs2 { sp_belongs, sp_belongs };
    std::vector<SeancePacket> all_notbelongs1 { sp_belongs, sp_notbelongs1 };
    std::vector<SeancePacket> all_notbelongs2 { sp_belongs, sp_notbelongs2 };
    std::vector<SeancePacket> all_notbelongs3 { sp_notbelongs1, sp_belongs };

    QVERIFY(sc1.BelongsTo(all_belongs1));
    QVERIFY(sc1.BelongsTo(all_belongs2));
    QVERIFY(!sc1.BelongsTo(all_notbelongs1));
    QVERIFY(!sc1.BelongsTo(all_notbelongs2));
    QVERIFY(!sc1.BelongsTo(all_notbelongs3));
}

void TestSeanceConfig::TestConvert() {
    QDateTime timestamp = QDateTime::currentDateTime();
    uint32_t dossier = 20000;
    uint16_t seance_id = 3;
    float dose = 13.7;
    float debit = 1.37;
    uint32_t mu_des = 455;
    uint32_t mu_cor = 0;
    uint32_t dur = 9500;

    SeanceConfig sc(QString("name"), dossier, seance_id, dose, debit, mu_des, mu_cor, dur, 300);
                                                                              // UM1  UM2
    SeancePacket p1(dossier, seance_id,     0, 400, 300, 350, 450, 390,   2,   1);
    SeancePacket p2(dossier, seance_id,     1, 410, 301, 351,   0, 390, 100, 100);
    SeancePacket p3(dossier, seance_id,     0, 500, 300, 354,   0, 500, 220, 200);
    SeancePacket p4(dossier, seance_id,     1, 700, 290, 344, 460, 390, 280, 300);
    SeancePacket p5(dossier + 1, seance_id,     1, 700, 290, 344, 460, 390, 280, 300);
    SeancePacket p6(dossier, seance_id + 1,     1, 700, 290, 344, 460, 390, 280, 300);

    std::vector<SeancePacket> sp1 { p1 };
    std::vector<SeancePacket> sp2 { p1, p2 };
    std::vector<SeancePacket> sp3 { p1, p2 , p3};
    std::vector<SeancePacket> sp4 { p1, p2 , p3, p4};
    std::vector<SeancePacket> spError1 { p1, p2, p3, p4, p5 };
    std::vector<SeancePacket> spError2 { p1, p2, p3, p4, p6 };

    std::vector<int> um1_del_1 {0};
    std::vector<int> um1_del_2 {0, 98};
    std::vector<int> um1_del_3 {0,98, 218};
    std::vector<int> um1_del_4 {0,98, 218, 278};

    std::vector<int> um2_del_1 {0};
    std::vector<int> um2_del_2 {0, 99};
    std::vector<int> um2_del_3 {0,99, 199};
    std::vector<int> um2_del_4 {0,99, 199, 299};

    std::vector<int> cf9_status_1 {0};
    std::vector<int> cf9_status_2 {0, 1};
    std::vector<int> cf9_status_3 {0, 1, 0};
    std::vector<int> cf9_status_4 {0, 1, 0, 1};

    std::vector<double> duration1 {0.4};
    std::vector<double> duration2 { 0.4, 0.41 };
    std::vector<double> duration3 {0.4, 0.41, 0.5};
    std::vector<double> duration4 {0.4, 0.41, 0.5, 0.7};

    std::vector<double> ich1_1 {300};
    std::vector<double> ich1_2 {300, 301};
    std::vector<double> ich1_3 {300, 301, 300};
    std::vector<double> ich1_4 {300, 301, 300, 290};

    std::vector<double> ich2_1 {350};
    std::vector<double> ich2_2 {350, 351};
    std::vector<double> ich2_3 {350, 351, 354};
    std::vector<double> ich2_4 {350, 351, 354, 344};

    SeanceRecord r1(timestamp, static_cast<double>(debit), duration1, ich1_1, ich2_1, cf9_status_1, 450.0, 390.0, static_cast<int>(mu_des), static_cast<int>(mu_cor), um1_del_1, um2_del_1, 0.0);
    SeanceRecord r2(timestamp, static_cast<double>(debit), duration2, ich1_2, ich2_2, cf9_status_2, 450.0, 390.0, static_cast<int>(mu_des), static_cast<int>(mu_cor), um1_del_2, um2_del_2, 0.0);
    SeanceRecord r3(timestamp, static_cast<double>(debit), duration3, ich1_3, ich2_3, cf9_status_3, 450.0, 390.0, static_cast<int>(mu_des), static_cast<int>(mu_cor), um1_del_3, um2_del_3, 0.0);
    SeanceRecord r4(timestamp, static_cast<double>(debit), duration4, ich1_4, ich2_4, cf9_status_4, 450.0, 390.0, static_cast<int>(mu_des), static_cast<int>(mu_cor), um1_del_4, um2_del_4, 0.0);

    QCOMPARE(sc.Convert(timestamp, sp1), r1);
    QCOMPARE(sc.Convert(timestamp, sp2), r2);
    QCOMPARE(sc.Convert(timestamp, sp3), r3);
    QCOMPARE(sc.Convert(timestamp, sp4), r4);
    try {
        sc.Convert(timestamp, spError1);
        QFAIL("Should not be possible to convert seancepackets which dont belong to the seanceconfig 1");
    }
    catch(std::exception& exc){}

    try {
        sc.Convert(timestamp, spError2);
        QFAIL("Should not be possible to convert seancepackets which dont belong to the seanceconfig 2");
    }
    catch(std::exception& exc){}

}

