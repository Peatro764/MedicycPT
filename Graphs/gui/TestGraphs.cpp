#include "TestGraphs.h"
#include "ui_TestGraphs.h"

#include <QMessageBox>
#include <QDebug>

TestGraphs::TestGraphs(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::TestGraphs)
{
    ui_->setupUi(this);
    setWindowTitle("Test Graphs");
    SetupGraphs();
    ConnectSignals();
}

TestGraphs::~TestGraphs() {}

void TestGraphs::SetupGraphs() {
    histo_ = std::shared_ptr<Histogram>(new Histogram(ui_->customPlot_histogram, QString(""),
                                                      QString("X"), -3.0, 3.0, 20));
    histo_->Register(QString("Test1"));
    histo_->Register(QString("Test2"));
    histo_->Register(QString("Test3"));

    time_series_ = std::shared_ptr<TimedStampedDataGraph>(new TimedStampedDataGraph(ui_->customPlot_time_series));
    time_series_->SetYAxisTitle("Yaxis");
    time_series_->Register("Test1");
    time_series_->AddPoint("Test1", 1000.0, 33.0, 4.4);
    time_series_->AddPoint("Test1", 2000.0, 29.0, 1.4);
    time_series_->AddPoint("Test1", 4000.0, 37.0, 10.4);

}

void TestGraphs::ConnectSignals() {
    QObject::connect(ui_->lineEdit_value_1, &QLineEdit::returnPressed, this, &TestGraphs::AddValue1);
    QObject::connect(ui_->lineEdit_value_2, &QLineEdit::returnPressed, this, &TestGraphs::AddValue2);
    QObject::connect(ui_->lineEdit_value_3, &QLineEdit::returnPressed, this, &TestGraphs::AddValue3);
    QObject::connect(ui_->pushButton_normalize, &QPushButton::pressed, this, &TestGraphs::Normalize);
}

void TestGraphs::AddValue1() {
    const double value = ui_->lineEdit_value_1->text().toDouble();
    histo_->SetSingleElement(QString("Test1"), value);
}

void TestGraphs::AddValue2() {
    const double value = ui_->lineEdit_value_2->text().toDouble();
    histo_->SetSingleElement(QString("Test2"), value);
}

void TestGraphs::AddValue3() {
    const double value = ui_->lineEdit_value_3->text().toDouble();
    histo_->SetSingleElement(QString("Test3"), value);
}

void TestGraphs::Normalize() {
    histo_->Normalize();
}
