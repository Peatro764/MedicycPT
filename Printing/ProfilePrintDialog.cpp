#include "ProfilePrintDialog.h"
#include "ui_ProfilePrintDialog.h"

#include <QDateTime>

#include "Calc.h"
#include "Hardware.h"

ProfilePrintDialog::ProfilePrintDialog(QWidget *parent, std::shared_ptr<QARepo> repo) :
    QDialog(parent),
    ui_(new Ui::ProfilePrintDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Profile"));
    SetupConnections();
    SetupGraphs();
    Fill();
}

ProfilePrintDialog::~ProfilePrintDialog()
{
    delete ui_;
}

void ProfilePrintDialog::SetupConnections() {
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ProfilePrintDialog::SetupGraphs() {
    profile_graph_ = std::unique_ptr<LinePlot>(new LinePlot(ui_->customPlot_Profile));
    profile_graph_->SetLightStyle();
    profile_graph_->InitCurve(AxisToString(Axis::X), Axis::X, 2, Qt::SolidLine);
    profile_graph_->InitCurve(AxisToString(Axis::Y), Axis::Y, 2, Qt::DashLine);
    profile_graph_->SetRange(-20.0, 20.0, 0.0, 0.1);

}

void ProfilePrintDialog::Fill() {
    try {
        BeamProfile pX = repo_->GetLastBeamProfile(Axis::X);
        BeamProfile pY = repo_->GetLastBeamProfile(Axis::Y);
        profile_graph_->SetCurves(AxisToString(Axis::X), pX);
        profile_graph_->SetCurves(AxisToString(Axis::Y), pY);
        BeamProfileResults rX = pX.GetResults();
        BeamProfileResults rY = pY.GetResults();
        ui_->lineEdit_hardware->setText(HardwareToString(pX.Hardware()));
        ui_->lineEdit_date->setText(pX.GetTimestamp().toString("yyyy:MM:dd-hh:mm"));
        ui_->lineEdit_x_timestamp->setText(pX.GetTimestamp().toString("yyyy:MM:dd-hh:mm"));
        ui_->lineEdit_x_centre->setText(QString::number(rX.centre(), 'f', 2));
        //    ui_->lineEdit_x_flatness->setText(QString::number(rX.flatness(), 'f', 2));
        ui_->lineEdit_x_l50->setText(QString::number(rX.width_50(), 'f', 2));
        ui_->lineEdit_x_l90->setText(QString::number(rX.width_90(), 'f', 2));
        ui_->lineEdit_x_l95->setText(QString::number(rX.width_95(), 'f', 2));
        ui_->lineEdit_x_penumbra->setText(QString::number(rX.penumbra(), 'f', 2));
        ui_->lineEdit_y_timestamp->setText(pY.GetTimestamp().toString("yyy:MM:dd-hh:mm"));
        ui_->lineEdit_y_centre->setText(QString::number(rY.centre(), 'f', 2));
        //    ui_->lineEdit_y_flatness->setText(QString::number(rY.flatness(), 'f', 2));
        ui_->lineEdit_y_l50->setText(QString::number(rY.width_50(), 'f', 2));
        ui_->lineEdit_y_l90->setText(QString::number(rY.width_90(), 'f', 2));
        ui_->lineEdit_y_l95->setText(QString::number(rY.width_95(), 'f', 2));
        ui_->lineEdit_y_penumbra->setText(QString::number(rY.penumbra(), 'f', 2));
        if (pX.Hardware() == QA_HARDWARE::SCANNER3D) {
            ui_->lineEdit_x_z->setText(QString::number(pX.AveragePosition(Axis::Z), 'f', 2));
        } else {
            ui_->lineEdit_x_z->setText("0");
        }
        if (pY.Hardware() == QA_HARDWARE::SCANNER3D) {
            ui_->lineEdit_y_z->setText(QString::number(pY.AveragePosition(Axis::Z), 'f', 2));
        } else {
            ui_->lineEdit_y_z->setText("0");
        }
    }
    catch (std::exception& exc) {
        qWarning() << "ProfilePrintDialog::Fill Exception thrown " << exc.what();
    }
}

QWidget* ProfilePrintDialog::Widget() {
    return ui_->profileWidget;
}

void ProfilePrintDialog::Print(QPrinter* printer) {
    printer->setOrientation(QPrinter::Portrait);
    printer->setPageSize(QPrinter::A4);
    printer->setFullPage(false);

    QCPPainter painter(printer);
    QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

    int plotWidth = ui_->profileWidget->width();
    int plotHeight = ui_->profileWidget->height();
    double w_scale = pageRect.width() / (double)plotWidth;
    double h_scale = pageRect.height() / (double)plotHeight;

    painter.scale(w_scale, h_scale);
    ui_->profileWidget->render(&painter);
    painter.end();
}
