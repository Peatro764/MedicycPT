#include "TestDegradeur.h"
#include "Degradeur.h"

using namespace QTest;

void TestDegradeur::Constructors() {
    Degradeur d1("1", 800, true);
    Degradeur d2("2", 1600, true);
    QCOMPARE((int)DegradeurSet(d1).GetIndividualDegradeurs().size(), 1);
    QCOMPARE((int)DegradeurSet(d1, d2).GetIndividualDegradeurs().size(), 2);
}

void TestDegradeur::UmPlexiTotal() {
    Degradeur d1("1", 800, true);
    Degradeur d2("2", 1600, true);
    QCOMPARE(DegradeurSet().um_plexi_total(), 0);
    QCOMPARE(DegradeurSet(d1).um_plexi_total(), 800);
    QCOMPARE(DegradeurSet(d1, d2).um_plexi_total(), 2400);
}

void TestDegradeur::UMPlexis() {
    Degradeur d1("1", 800, true);
    Degradeur d2("2", 1600, true);
    QStringList d1_umplexis("800");
    QStringList d1d2_umplexis;
    d1d2_umplexis << "800" << "1600";

    QCOMPARE(DegradeurSet(d1).um_plexis() , d1_umplexis);
    QCOMPARE(DegradeurSet(d1, d2).um_plexis(), d1d2_umplexis);
}

void TestDegradeur::MMPlexis() {
    Degradeur d1("1", 800, true);
    Degradeur d2("2", 1600, true);
    QStringList d1_mmplexis("0.8");
    QStringList d1d2_mmplexis;
    d1d2_mmplexis << "0.8" << "1.6";

    QCOMPARE(DegradeurSet(d1).mm_plexis() , d1_mmplexis);
    QCOMPARE(DegradeurSet(d1, d2).mm_plexis(), d1d2_mmplexis);
}

void TestDegradeur::Ids() {
    Degradeur d1("1", 800, true);
    Degradeur d2("2", 1600, true);
    QStringList d1_ids("1");
    QStringList d1d2_ids;
    d1d2_ids << "1" << "2";

    QCOMPARE(DegradeurSet(d1).ids(), d1_ids);
    QCOMPARE(DegradeurSet(d1, d2).ids(), d1d2_ids);
}

void TestDegradeur::Comparison() {
    Degradeur d1("1", 800, true);
    Degradeur d2("2", 1600, true);

    QVERIFY(!(d1 == d2));
    QVERIFY(d1 == d1);
    QVERIFY(!(DegradeurSet(d1, d2) == DegradeurSet(d1)));
    QVERIFY(!(DegradeurSet(d1, d1) == DegradeurSet(d1, d2)));
    QVERIFY(DegradeurSet(d1, d2) == DegradeurSet(d1, d2));
    QVERIFY(DegradeurSet(d1) == DegradeurSet(d1));
    QCOMPARE(d1.mm_plexi(), 0.8);
    QCOMPARE(d2.mm_plexi(), 1.6);
}
