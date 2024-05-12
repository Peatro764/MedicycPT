#include "TestModulateur.h"
#include "Modulateur.h"

#include "Calc.h"

using namespace QTest;

void TestModulateur::Constructors() {
  const QString id("13");
  const double mod_100(44.0);
  const double mod_98(55.0);
  const double parcours(33.0);
  const int um_degradeur(345);
  const bool available(false);
  std::map<int, double> map1;
  map1[0] = 100.0;
  map1[800] = 50.0;
  map1[1600] = 70.0;
  Modulateur mod1(id, mod_100, mod_98, parcours, um_degradeur, map1, available);

  QCOMPARE(mod1.id(), id);
  QCOMPARE(mod1.steps(), 2);
  QCOMPARE(mod1.mod100(), mod_100);
  QCOMPARE(mod1.mod98(), mod_98);
  QCOMPARE(mod1.parcours(), parcours);
  QCOMPARE(mod1.RangeShift(), um_degradeur);
  QCOMPARE(mod1.um_plexi_weight(), map1);
  QCOMPARE(mod1.available(), available);

  QString opt_alg("optalg");
  QString data_set("dataset");
  int n_sectors(3);
  double mod_step(0.8);
  double input_parcours(5.5);
  double input_mod(3.0);
  double decalage(0.4);
  double conv_criteria(0.0001);

   Modulateur mod2(id, mod_100, mod_98, parcours, um_degradeur, map1, available,
                   opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod,
                   decalage, conv_criteria);
   QCOMPARE(mod2.id(), id);
   QCOMPARE(mod2.steps(), 2);
   QCOMPARE(mod2.mod100(), mod_100);
   QCOMPARE(mod2.mod98(), mod_98);
   QCOMPARE(mod2.parcours(), parcours);
   QCOMPARE(mod2.RangeShift(), um_degradeur);
   QCOMPARE(mod2.um_plexi_weight(), map1);
   QCOMPARE(mod2.available(), available);
   QCOMPARE(mod2.opt_alg(), opt_alg);
   QCOMPARE(mod2.data_set(), data_set);
   QCOMPARE(mod2.n_sectors(), n_sectors);
   QCOMPARE(mod2.mod_step(), mod_step);
   QCOMPARE(mod2.input_parcours(), input_parcours);
   QCOMPARE(mod2.input_mod(), input_mod);
   QCOMPARE(mod2.decalage(), decalage);
   QCOMPARE(mod2.conv_criteria(), conv_criteria);
}

void TestModulateur::Comparison() {
    std::map<int, double> map1;
    map1[1] = 100.0;
    map1[5] = 50.0;
    map1[7] = 70.0;
    std::map<int, double> map2;
    map2[1] = 100.0;
    map2[5] = 55.0;
    map2[7] = 70.0;
    Modulateur mod1(QString("1"), 1.0, 2.0, 3.0, 345, map1, true);
    Modulateur mod2(QString("1"), 1.0, 2.0, 3.0, 345, map2, true);
    QCOMPARE(mod1, mod1);
    QVERIFY(mod1 != mod2);
}

void TestModulateur::steps() {
    std::map<int, double> map1;
    map1[0] = 1.0;
    map1[800] = 1.0;
    map1[1600] = 1.0;
    Modulateur mod1(QString("1"), 1.0, 2.0, 3.0, 1000, map1, true);
    QCOMPARE(mod1.steps(), 2);

    std::map<int, double> map2;
    map2[1600] = 1.0;
    map2[2400] = 1.0;
    map2[3200] = 1.0;
    Modulateur mod2(QString("1"), 1.0, 2.0, 3.0, 1000, map2, true);
    QCOMPARE(mod2.steps(), 2);

    std::map<int, double> map3;
    map3[1600] = 1.0;
    Modulateur mod3(QString("1"), 1.0, 2.0, 3.0, 444, map3, true);
    QCOMPARE(mod3.steps(), 0);

    std::map<int, double> map4;
    Modulateur mod4(QString("1"), 1.0, 2.0, 3.0, 555, map4, true);
    QCOMPARE(mod4.steps(), 0);

    Modulateur mod5;
    QCOMPARE(mod5.steps(), 0);
}

void TestModulateur::GetSectorAngularSizes() {
    std::map<int, double> weights;
    weights.emplace(0, 1.0);
    weights.emplace(1000, 0.5);
    weights.emplace(4000, 0.3);
    weights.emplace(5000, 0.11);

    std::map<int, double> angular_sizes_4sectors(Modulateur::GetSectorAngularSizes(weights, 4));
    std::map<int, double> exp_angular_sizes_4sectors;
    exp_angular_sizes_4sectors.emplace(0, 90.0);
    exp_angular_sizes_4sectors.emplace(1000, 42.8796);
    exp_angular_sizes_4sectors.emplace(4000, 19.3194);
    exp_angular_sizes_4sectors.emplace(5000, 5.1832);
    for (auto&w : angular_sizes_4sectors) {
        QVERIFY(calc::AlmostEqual(w.second, exp_angular_sizes_4sectors.at(w.first), 0.001));
    }

    std::map<int, double> angular_sizes_2sectors(Modulateur::GetSectorAngularSizes(weights, 2));
    std::map<int, double> exp_angular_sizes_2sectors;
    exp_angular_sizes_2sectors.emplace(0, 2.0 * 90.0);
    exp_angular_sizes_2sectors.emplace(1000, 2.0 * 42.8796);
    exp_angular_sizes_2sectors.emplace(4000, 2.0 * 19.3194);
    exp_angular_sizes_2sectors.emplace(5000, 2.0 * 5.1832);
    for (auto&w : angular_sizes_2sectors) {
        QVERIFY(calc::AlmostEqual(w.second, exp_angular_sizes_2sectors.at(w.first), 0.00001));
    }
}

void TestModulateur::GetSectorAngularSizesNonDecreasingWeights() {
    std::map<int, double> weights;
    weights.emplace(0, 1.0);
    weights.emplace(1000, 0.3);
    weights.emplace(4000, 0.4);
    weights.emplace(5000, 0.5);

    std::map<int, double> angular_sizes_4sectors(Modulateur::GetSectorAngularSizes(weights, 4));
    std::map<int, double> exp_angular_sizes_4sectors;
    exp_angular_sizes_4sectors.emplace(0, 90.0);
    exp_angular_sizes_4sectors.emplace(1000, 49.09086);
    exp_angular_sizes_4sectors.emplace(4000, 36.8181);
    exp_angular_sizes_4sectors.emplace(5000, 20.4545);
    for (auto&w : angular_sizes_4sectors) {
        QVERIFY(calc::AlmostEqual(w.second, exp_angular_sizes_4sectors.at(w.first), 0.00001));
    }
}


