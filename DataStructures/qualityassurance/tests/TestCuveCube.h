#include <QtTest/QtTest>

#include "CuveCube.h"

class TestCuveCube : public QObject {
    Q_OBJECT
private:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void getters();
    void configs();
    void points();
    void colormap_dimensions();
    void colormap_content();
};


