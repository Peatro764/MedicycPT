#include "TestColorHandler.h"
#include "ColorHandler.h"

void TestColorHandler::initTestCase() {}

void TestColorHandler::cleanupTestCase() {}

void TestColorHandler::init() {}

void TestColorHandler::cleanup() {}

void TestColorHandler::GenerateColors() {
    QCOMPARE((int)colorhandler::GenerateColors(0).size(), 0);
    QCOMPARE((int)colorhandler::GenerateColors(1).size(), 1);
    QCOMPARE((int)colorhandler::GenerateColors(2).size(), 2);
    QCOMPARE((int)colorhandler::GenerateColors(3).size(), 3);
    QCOMPARE((int)colorhandler::GenerateColors(50).size(), 50);
}

