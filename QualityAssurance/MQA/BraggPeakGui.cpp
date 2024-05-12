#include "BraggPeakGui.h"
#include "ui_MQA.h"
#include "MQA.h"

#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>

#include "Axis.h"
#include "Hardware.h"
#include "Calc.h"
#include "Material.h"
#include "ManualMotorControlDialog.h"
#include "LinePlotMeasurementDialog.h"
#include "DepthDoseTypeDialog.h"
#include "FetchSOBPDialog.h"
#include "SOBPResultsDialog.h"

BraggPeakGui::BraggPeakGui(MQA *parent, std::shared_ptr<QARepo> repo, QA_HARDWARE hardware) :
    GuiBase(parent),
    repo_(repo),
    bp_history_page_(parent, repo),
    sobp_history_page_(parent, repo, 10000),
    hardware_(hardware)
{
    dev_factory_ = std::shared_ptr<DevFactory>(new DevFactory(repo_));
    braggpeaker_sm_  = std::shared_ptr<BraggPeaker>(new BraggPeaker(dev_factory_, &abort_beacon_, hardware_));
    SetupGraphs();
    SetupBraggPeak();
    ConnectSignals();
    SetDefaults(); // Must come after connect signals!
    SetupValidators();
    SetupTimer();
    parent_->ui()->groupBox_b_dateInterval->setVisible(false);
    parent_->ui()->groupBox_b_dateInterval->setEnabled(false);
}

BraggPeakGui::~BraggPeakGui() {
    qDebug() << "~BraggPeakGui";
    TurnOffConnectedButton();
}

void BraggPeakGui::SetDefaults() {
    parent_->ui()->b_lineEdit_signal_noise->setText("0.0");
    parent_->ui()->b_lineEdit_x_pos->setText("-");
    parent_->ui()->b_lineEdit_y_pos->setText("-");
    parent_->ui()->b_lineEdit_z_pos->setText("-");
    parent_->ui()->b_lineEdit_width_plexi->setText("-");
    parent_->ui()->b_lineEdit_width_tissue->setText("-");
    parent_->ui()->b_lineEdit_penumbra_plexi->setText("-");
    parent_->ui()->b_lineEdit_penumbra_tissue->setText("-");
    parent_->ui()->b_lineEdit_parcours_plexi->setText("-");
    parent_->ui()->b_lineEdit_parcours_tissue->setText("-");
    parent_->ui()->b_lineEdit_mod98_plexi->setText("-");
    parent_->ui()->b_lineEdit_mod98_tissue->setText("-");
    parent_->ui()->b_lineEdit_mod100_plexi->setText("-");
    parent_->ui()->b_lineEdit_mod100_tissue->setText("-");

    parent_->ui()->b_dateEdit_from->setDisplayFormat("yyyy.MM.dd");

    parent_->ui()->b_dateEdit_to->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->b_dateEdit_from->setCurrentSection(QDateTimeEdit::MonthSection);
    parent_->ui()->b_dateEdit_to->setCurrentSection(QDateTimeEdit::MonthSection);
    QDateTime current_date(QDateTime::currentDateTime());
    parent_->ui()->b_dateEdit_from->setDate(current_date.date().addDays(-182));
    parent_->ui()->b_dateEdit_to->setDate(current_date.date());
}

void BraggPeakGui::SetRanges() {
    QString prefix;

    parent_->ui()->b_doubleSpinBox_step->setDecimals(3);
    parent_->ui()->b_doubleSpinBox_range_lower->setDecimals(3);
    parent_->ui()->b_doubleSpinBox_range_upper->setDecimals(3);

    switch(hardware_) {
    case QA_HARDWARE::WHEEL:
        prefix = "wheel/";
        parent_->ui()->b_doubleSpinBox_step->setReadOnly(true);
        parent_->ui()->b_doubleSpinBox_step->setValue(GetSetting(QString("z_step").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_lower->setMinimum(GetSetting(QString("z_min").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_lower->setValue(GetSetting(QString("z_min").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_lower->setSingleStep(GetSetting(QString("z_step").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_upper->setMaximum(GetSetting(QString("z_max").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_upper->setValue(GetSetting(QString("z_max").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_upper->setSingleStep(GetSetting(QString("z_step").prepend(prefix)).toDouble());
        parent_->ui()->b_label_max_range_unit->setText("mm plexi");
        parent_->ui()->b_label_min_range_unit->setText("mm plexi");
        parent_->ui()->b_label_step_unit->setText("mm plexi");
        break;
    case QA_HARDWARE::SCANNER3D:
        prefix = "scanner3D/";
        parent_->ui()->b_doubleSpinBox_step->setReadOnly(false);
        parent_->ui()->b_doubleSpinBox_step->setValue(GetSetting(QString("z_step").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_lower->setMinimum(GetSetting(QString("z_min").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_lower->setSingleStep(GetSetting(QString("z_step").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_lower->setValue(GetSetting(QString("z_min").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_upper->setMaximum(GetSetting(QString("z_max").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_upper->setSingleStep(GetSetting(QString("z_step").prepend(prefix)).toDouble());
        parent_->ui()->b_doubleSpinBox_range_upper->setValue(GetSetting(QString("z_max").prepend(prefix)).toDouble());
        parent_->ui()->b_label_max_range_unit->setText("mm eau");
        parent_->ui()->b_label_min_range_unit->setText("mm eau");
        parent_->ui()->b_label_step_unit->setText("mm eau");
        break;
    default:
        qWarning() << "BraggPeakGui::SetRanges non valid hardware";
        parent_->ui()->b_doubleSpinBox_step->setValue(0);
        parent_->ui()->b_doubleSpinBox_range_lower->setSingleStep(0);
        parent_->ui()->b_doubleSpinBox_range_upper->setSingleStep(0);
        parent_->ui()->b_doubleSpinBox_range_lower->setMinimum(0);
        parent_->ui()->b_doubleSpinBox_range_upper->setMaximum(0);
        parent_->ui()->b_label_max_range_unit->setText("?");
        parent_->ui()->b_label_min_range_unit->setText("?");
        parent_->ui()->b_label_step_unit->setText("?");
        break;
    }
}

void BraggPeakGui::SetupValidators() {
    parent_->ui()->b_lineEdit_width_tissue->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->b_lineEdit_width_tissue));
    parent_->ui()->b_lineEdit_penumbra_tissue->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->b_lineEdit_penumbra_tissue));
    parent_->ui()->b_lineEdit_parcours_tissue->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->b_lineEdit_parcours_tissue));
    parent_->ui()->b_lineEdit_mod98_tissue->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->b_lineEdit_mod98_tissue));
    parent_->ui()->b_lineEdit_mod100_tissue->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->b_lineEdit_mod100_tissue));
}

void BraggPeakGui::SetupGraphs() {
    parent_->ui()->b_label_chambre_voltage->setAlignment(Qt::AlignCenter);
    parent_->ui()->b_label_diode_voltage->setAlignment(Qt::AlignCenter);

    braggPeak_graph_ = std::shared_ptr<LinePlot>(new LinePlot(parent_->ui()->b_customPlot_braggPeak));
    braggPeak_graph_->InitCurve(measured_graph_name_, Axis::Z, 1, Qt::SolidLine);
//    braggPeak_graph_->InitCurve(uploaded_graph_name_, Axis::Z, 1);
    braggPeak_graph_->SetRange(0.0, 35.0, 0.0, 0.1);
    braggPeak_graph_->SetAxisLabels("Position [mm tissue]", "Intensity");
    braggPeak_graph_->ShowLegend(false);
}

void BraggPeakGui::SetupBraggPeak() {
   depth_dose_ = std::shared_ptr<DepthDoseMeasurement>(new DepthDoseMeasurement(hardware_));
}

void BraggPeakGui::ConnectSignals() {
    QObject::connect(parent_->ui()->b_lineEdit_width_tissue, &QLineEdit::editingFinished, this, [&]() {
        parent_->ui()->b_lineEdit_width_plexi->setText(QString::number(material::ToPlexiglas(parent_->ui()->b_lineEdit_width_tissue->text().toDouble(), material::MATERIAL::TISSUE), 'f', 2)); });
    QObject::connect(parent_->ui()->b_lineEdit_parcours_tissue, &QLineEdit::editingFinished, this, [&]() {
        parent_->ui()->b_lineEdit_parcours_plexi->setText(QString::number(material::ToPlexiglas(parent_->ui()->b_lineEdit_parcours_tissue->text().toDouble(), material::MATERIAL::TISSUE), 'f', 2)); });
    QObject::connect(parent_->ui()->b_lineEdit_penumbra_tissue, &QLineEdit::editingFinished, this, [&]() {
        parent_->ui()->b_lineEdit_penumbra_plexi->setText(QString::number(material::ToPlexiglas(parent_->ui()->b_lineEdit_penumbra_tissue->text().toDouble(), material::MATERIAL::TISSUE), 'f', 2)); });
    QObject::connect(parent_->ui()->b_lineEdit_mod98_tissue, &QLineEdit::editingFinished, this, [&]() {
        parent_->ui()->b_lineEdit_mod98_plexi->setText(QString::number(material::ToPlexiglas(parent_->ui()->b_lineEdit_mod98_tissue->text().toDouble(), material::MATERIAL::TISSUE), 'f', 2)); });
    QObject::connect(parent_->ui()->b_lineEdit_mod100_tissue, &QLineEdit::editingFinished, this, [&]() {
        parent_->ui()->b_lineEdit_mod100_plexi->setText(QString::number(material::ToPlexiglas(parent_->ui()->b_lineEdit_mod100_tissue->text().toDouble(), material::MATERIAL::TISSUE), 'f', 2)); });

    QObject::connect(parent_->ui()->b_pushButton_results_updatePlot, &QPushButton::clicked, this, &BraggPeakGui::UpdateResultsPlot);
    QObject::connect(parent_->ui()->b_pushButton_connected, &QPushButton::clicked, this, &BraggPeakGui::Configure);
    QObject::connect(parent_->ui()->b_pushButton_save, &QPushButton::clicked, this, &BraggPeakGui::SaveToDb);
    QObject::connect(parent_->ui()->b_pushButton_uploadSOBP, &QPushButton::clicked, this, &BraggPeakGui::UploadSOBP);
    QObject::connect(parent_->ui()->b_pushButton_clearUploadedSOBP, &QPushButton::clicked, this, &BraggPeakGui::ClearUploadedSOBP);
    QObject::connect(parent_->ui()->b_pushButton_normalize, &QPushButton::clicked, this, &BraggPeakGui::Normalize);

    QObject::connect(&abort_beacon_, &Beacon::BroadcastedMessage, this, &BraggPeakGui::DisplayError);

    // tabwidget
    QObject::connect(parent_->ui()->b_tabWidget, &QTabWidget::currentChanged, this, &BraggPeakGui::TabWidgetPageChanged);

    // Signal noise
    QObject::connect(parent_->ui()->b_pushButton_read_signal_noise, &QPushButton::pressed, this, &BraggPeakGui::ReadSignalNoise);
    QObject::connect(this, &BraggPeakGui::SIGNAL_AxisDeviation, this, [&](Axis axis, double value) {
        if (axis == Axis::X) parent_->ui()->b_lineEdit_x_diff->setText(QString::number(value, 'f', 2));
        if (axis == Axis::Y) parent_->ui()->b_lineEdit_y_diff->setText(QString::number(value, 'f', 2));
        if (axis == Axis::Z) parent_->ui()->b_lineEdit_z_diff->setText(QString::number(value, 'f', 2));
    });

    // Profile Acquisition
    QObject::connect(parent_->ui()->b_pushButton_start, &QPushButton::clicked, this, &BraggPeakGui::StartAcquisition);
    QObject::connect(parent_->ui()->b_pushButton_abort, &QPushButton::clicked, this, &BraggPeakGui::AbortAcquisition);
    QObject::connect(parent_->ui()->b_pushButton_movetozero, &QPushButton::clicked, this, &BraggPeakGui::MovingToZero);
    QObject::connect(braggpeaker_sm_.get(), &BraggPeaker::Finished, this, &BraggPeakGui::DisplayBraggPeak);
    QObject::connect(braggpeaker_sm_.get(), &BraggPeaker::Finished, this, [&]() { if (hardware_ == QA_HARDWARE::SCANNER3D) VerifyAxesCalibrations(); });
    QObject::connect(braggpeaker_sm_.get(), &BraggPeaker::Measurement, this, &BraggPeakGui::AddDataPointToContainer);
    QObject::connect(braggpeaker_sm_.get(), &BraggPeaker::Measurement, this, &BraggPeakGui::AddDataPointToGraph);

    QObject::connect(braggpeaker_sm_.get(), &BraggPeaker::Measurement, this, [=](MeasurementPoint p) {
        parent_->ui()->b_label_chambre_voltage->setText(QString::number(abs(p.signal().reference()), 'f', 2));
    });
    QObject::connect(braggpeaker_sm_.get(), &BraggPeaker::Measurement, this, [=](MeasurementPoint p) {
        parent_->ui()->b_label_diode_voltage->setText(QString::number(abs(p.signal().raw()), 'f', 2));
    });
    QObject::connect(braggpeaker_sm_.get(), &BraggPeaker::SIGNAL_DiodeVoltage, this, [&](double value) {
        parent_->ui()->b_label_diode_voltage->setText(QString::number(abs(value), 'f', 2));
    });
    QObject::connect(braggpeaker_sm_.get(), &BraggPeaker::SIGNAL_ChambreVoltage, this, [&](double value) {
        parent_->ui()->b_label_chambre_voltage->setText(QString::number(abs(value), 'f', 2));
    });

    // Ranges
    QObject::connect(parent_->ui()->b_doubleSpinBox_range_lower, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, [=](double d) { LowerScanRangeChanged(d); });
    QObject::connect(parent_->ui()->b_doubleSpinBox_range_upper, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, [=](double d) { UpperScanRangeChanged(d); });
}

void BraggPeakGui::UpdateResultsPlot() {
    QDate from_date(parent_->ui()->b_dateEdit_from->date());
    QDate to_date(parent_->ui()->b_dateEdit_to->date());

    switch (static_cast<TABWIDGETPAGE>(parent_->ui()->b_tabWidget->currentIndex())) {
    case TABWIDGETPAGE::BP_RESULTS:
        bp_history_page_.FetchData(from_date, to_date);
        break;
    case TABWIDGETPAGE::SOBP_RESULTS:
        sobp_history_page_.FetchData(from_date, to_date);
        break;
    default:
        break;
    }
}

void BraggPeakGui::GetSOBPs() {
    qDebug() << "GetSOBPs";
    std::vector<SOBP> sobps = repo_->GetSOBPs();
    for (SOBP& s : sobps) {
        qDebug() << "SOBP " << s.dossier() << " " << s.modulateur_id() << " " << s.comment();
        DepthDoseResults d = s.depth_dose().GetResults();
    }
}

void BraggPeakGui::MovingToZero() {
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before moving to start position");
        return;
    }
    if (hardware_ == QA_HARDWARE::SCANNER3D) {
        if (!VerifyAxesCalibrations()) {
            return;
        }
    }
    emit braggpeaker_sm_->MovingToZero();
}

void BraggPeakGui::Configure() {
    if (!IsIdle()) {
        AbortAcquisition();
        //DisplayWarning("Please stop axes before reconfiguring hardware");
        //return;
    }

    try {
        parent_->ui()->b_pushButton_manual_control_x->setEnabled(hardware_ == QA_HARDWARE::SCANNER3D);
        parent_->ui()->b_pushButton_manual_control_y->setEnabled(hardware_ == QA_HARDWARE::SCANNER3D);

        SetRanges();
        TurnOffConnectedButton();
        dev_factory_->Setup(hardware_);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition, this, 0);
        QObject::connect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition,
                         this, [&](double pos) { parent_->ui()->b_lineEdit_z_pos->setText(QString::number(pos, 'f', 2)); });

        QObject::disconnect(parent_->ui()->b_pushButton_manual_control_z, 0, 0, 0);
        QObject::connect(parent_->ui()->b_pushButton_manual_control_z, &QPushButton::clicked, this, [=](){
            LaunchManualMotorControlDialog(Axis::Z, dev_factory_->GetMotor(Axis::Z)); });

        TurnOnConnectedButton();
        parent_->ui()->b_lineEdit_z_pos->setText(QString::number(dev_factory_->GetMotor(Axis::Z)->ActPos(), 'f', 2));

        if (hardware_ == QA_HARDWARE::SCANNER3D) {
            QObject::disconnect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition, this, 0);
            QObject::disconnect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition, this, 0);
            QObject::disconnect(parent_->ui()->b_pushButton_manual_control_x, 0, 0, 0);
            QObject::disconnect(parent_->ui()->b_pushButton_manual_control_y, 0, 0, 0);

            QObject::connect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition,
                             this, [&](double pos) { parent_->ui()->b_lineEdit_x_pos->setText(QString::number(pos, 'f', 2)); });
            QObject::connect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition,
                             this, [&](double pos) { parent_->ui()->b_lineEdit_y_pos->setText(QString::number(pos, 'f', 2)); });
            QObject::connect(parent_->ui()->b_pushButton_manual_control_x, &QPushButton::clicked, this, [=](){
                LaunchManualMotorControlDialog(Axis::X, dev_factory_->GetMotor(Axis::X)); });
            QObject::connect(parent_->ui()->b_pushButton_manual_control_y, &QPushButton::clicked, this, [=](){
                LaunchManualMotorControlDialog(Axis::Y, dev_factory_->GetMotor(Axis::Y)); });
            parent_->ui()->b_lineEdit_x_pos->setText(QString::number(dev_factory_->GetMotor(Axis::X)->ActPos(), 'f', 2));
            parent_->ui()->b_lineEdit_y_pos->setText(QString::number(dev_factory_->GetMotor(Axis::Y)->ActPos(), 'f', 2));
            dev_factory_->CalibrateAxisPosition(Axis::X);
            dev_factory_->CalibrateAxisPosition(Axis::Y);
            dev_factory_->CalibrateAxisPosition(Axis::Z);
        }

        if (!signal_noise_read_once_) {
            this->ReadSignalNoise();
            signal_noise_read_once_ = true;
        }
    }
    catch (std::exception& exc) {
        TurnOffConnectedButton();
        QString msg = QString("Failed configuring hardware: ") + exc.what();
        DisplayCritical(msg);
    }
}

void BraggPeakGui::VerifyAxisCalibration(Axis axis) {
    try {
        double diff;
        bool ok = dev_factory_->VerifyAxisPositionCalibration(axis, diff);
        emit SIGNAL_AxisDeviation(axis, diff);

        qDebug() << "BraggPeakGui::VerifyAxisCalibration " << AxisToString(axis) << " " << diff;
        if (!ok) {
            QString message = AxisToString(axis) + "-axis has a larger position deviation than allowed. Consider recalibrating axis.\n";
            message.append("\ndelta = ").append(QString::number(diff, 'f', 2));
            DisplayModelessMessageBox(message, false, 0, QMessageBox::Warning);
        }
        qDebug() << "BraggPeakGui::VerifyAxisCalibration " << AxisToString(axis) << " delta = " << diff;
    } catch (std::exception& exc) {
        QString msg = QString("Failed verifying axis calibrations: ") + exc.what();
        DisplayCritical(msg);
        return;
    }
}

bool BraggPeakGui::VerifyAxesCalibrations() {
    try {
        double x_diff, y_diff, z_diff;
        bool x_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::X, x_diff);
        bool y_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::Y, y_diff);
        bool z_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::Y, z_diff);

        emit SIGNAL_AxisDeviation(Axis::X, x_diff);
        emit SIGNAL_AxisDeviation(Axis::Y, y_diff);
        emit SIGNAL_AxisDeviation(Axis::Z, z_diff);

        qDebug() << "BraggPeakGui::VerifyAxesCalibrations (X, Y, Z) " << x_diff << " " << y_diff << " " << z_diff;

        if (x_ok && y_ok && z_ok) {
            return true;
        } else {
            QString message = "One or several axes has a larger position deviation than allowed. Consider recalibrating axes.\n";
            message.append("\ndX = ").append(QString::number(x_diff, 'f', 2))
                    .append("\ndY = ").append(QString::number(y_diff, 'f', 2))
                    .append("\ndZ = ").append(QString::number(z_diff, 'f', 2));
            QMessageBox msgBox;
            msgBox.setText(message);
            msgBox.setInformativeText("Continue anyway?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            return (msgBox.exec() == QMessageBox::Yes);
        }
    } catch (std::exception& exc) {
        QString msg = QString("Failed verifying axes calibrations: ") + exc.what();
        DisplayCritical(msg);
        return false;
    }
}

void BraggPeakGui::AddDataPointToContainer(MeasurementPoint m) {    
    *depth_dose_.get() << m.ScaleAxis(Axis::Z, material::ToTissue(1.0, IsMadeOf(hardware_)));
}

void BraggPeakGui::AddDataPointToGraph(MeasurementPoint m) {
    if (++graph_update_counter_ == 1) {
        graph_update_counter_ = 0;
        braggPeak_graph_->AddPoint(measured_graph_name_,
                                   m.ScaleAxis(Axis::Z, material::ToTissue(1.0, IsMadeOf(hardware_))),
                                   depth_dose_->GetSignalNoise());
    }
}

void BraggPeakGui::CleanUp() {
      try {
          qDebug() << "BraggPeakGui::CleanUp";
          emit braggpeaker_sm_->StopStateMachine();
          dev_factory_->CleanUp();
       }
       catch (std::exception& exc) {
           qDebug() << "BraggPeakGui::CleanUp Exception caught " << exc.what();
           DisplayError(QString("Failed cleaning up hardware:\n\n") + exc.what());
     }
}

bool BraggPeakGui::IsIdle() const {
    return braggpeaker_sm_->IsIdle();
}

void BraggPeakGui::LowerScanRangeChanged(double value) {
    braggPeak_graph_->SetHorLowerRange(-5.0 + value);
}

void BraggPeakGui::UpperScanRangeChanged(double value) {
    braggPeak_graph_->SetHorUpperRange(5.0 + value);
}

Range BraggPeakGui::GetRange() const {
    return Range(parent_->ui()->b_doubleSpinBox_range_lower->value(),
                 parent_->ui()->b_doubleSpinBox_range_upper->value());
}

double BraggPeakGui::GetStep() const {
    return parent_->ui()->b_doubleSpinBox_step->value();
}

double BraggPeakGui::GetNmbMeasurements() const {
    return parent_->ui()->b_spinBox_n_measurements->value();
}

double BraggPeakGui::GetMaxStdError() const {
    return parent_->ui()->b_spinBox_sem->value();
}

bool BraggPeakGui::GetFixedNmbMeasurements()  const {
    return parent_->ui()->b_radioButton_enable_fixed_nmb_measurements->isChecked();
}

void BraggPeakGui::StartAcquisition() {
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before starting a new measurement");
        return;
    }
    if (hardware_ == QA_HARDWARE::SCANNER3D) {
        if (!VerifyAxesCalibrations()) {
            return;
        }
    }
    ClearAll();
    braggpeaker_sm_->Start(GetRange(), GetStep(), GetNmbMeasurements(),
                           GetMaxStdError(), GetFixedNmbMeasurements());
}

void BraggPeakGui::AbortAcquisition() {
    qDebug() << "BraggPeakGui::AbortAcquisition";
    abort_beacon_.Activate();
    DisplayBraggPeak();
}

void BraggPeakGui::ClearAll() {
    depth_dose_->Clear();
    ClearGraphs();
    ClearResults();
}

void BraggPeakGui::ClearGraphs() {
    try {
        braggPeak_graph_->Clear(measured_graph_name_);
    }
    catch (std::exception& exc) {
        qWarning() << "BraggPeakGui::ClearGraphs Exception thrown: " << exc.what();
    }
}

void BraggPeakGui::ClearResults() {
    parent_->ui()->b_lineEdit_width_tissue->setText("-");
    parent_->ui()->b_lineEdit_penumbra_tissue->setText("-");
    parent_->ui()->b_lineEdit_parcours_tissue->setText("-");
    parent_->ui()->b_lineEdit_mod98_tissue->setText("-");
    parent_->ui()->b_lineEdit_mod100_tissue->setText("-");
    parent_->ui()->b_lineEdit_width_plexi->setText("-");
    parent_->ui()->b_lineEdit_penumbra_plexi->setText("-");
    parent_->ui()->b_lineEdit_parcours_plexi->setText("-");
    parent_->ui()->b_lineEdit_mod98_plexi->setText("-");
    parent_->ui()->b_lineEdit_mod100_plexi->setText("-");
}

void BraggPeakGui::DisplayBraggPeak() {
    qDebug() << "BraggPeakGui::DisplayBraggPeak";
    UpdateGraphs();
    UpdateResults(measured_graph_name_, *depth_dose_.get());
}

void BraggPeakGui::UpdateGraphs() {
    try {
        braggPeak_graph_->SetCurves(measured_graph_name_, *depth_dose_.get());
   }
    catch (std::exception& exc) {
        braggPeak_graph_->Clear(measured_graph_name_);
        qDebug() << "BraggPeakGui::UpdateGraphs " << exc.what();
        DisplayError(QString("Failed updating graph"));
    }
}

void BraggPeakGui::UpdateResults(QString name, const DepthDoseMeasurement& depth_dose) {
    qDebug() << "BraggPeakGui::UpdateResults";
    if (depth_dose.IsEmpty()) {
        ClearResults();
        return;
    }
    try {
        DepthDoseResults results_tissue = depth_dose.GetResults();
        parent_->ui()->b_lineEdit_width_tissue->setText(QString::number(results_tissue.width50(), 'f', 2));
        parent_->ui()->b_lineEdit_penumbra_tissue->setText(QString::number(results_tissue.penumbra(), 'f', 2));
        parent_->ui()->b_lineEdit_parcours_tissue->setText(QString::number(results_tissue.parcours(), 'f', 2));
        parent_->ui()->b_lineEdit_mod98_tissue->setText(QString::number(results_tissue.mod98(), 'f', 2));
        parent_->ui()->b_lineEdit_mod100_tissue->setText(QString::number(results_tissue.mod100(), 'f', 2));

        DepthDoseResults results_plexi = depth_dose.ScaleAxis(Axis::Z, material::ToPlexiglas(1.0, material::MATERIAL::TISSUE)).GetResults();
        parent_->ui()->b_lineEdit_width_plexi->setText(QString::number(results_plexi.width50(), 'f', 2));
        parent_->ui()->b_lineEdit_penumbra_plexi->setText(QString::number(results_plexi.penumbra(), 'f', 2));
        parent_->ui()->b_lineEdit_parcours_plexi->setText(QString::number(results_plexi.parcours(), 'f', 2));
        parent_->ui()->b_lineEdit_mod98_plexi->setText(QString::number(results_plexi.mod98(), 'f', 2));
        parent_->ui()->b_lineEdit_mod100_plexi->setText(QString::number(results_plexi.mod100(), 'f', 2));

        braggPeak_graph_->RemoveIndicatorPoints(measured_graph_name_);
        if (depth_dose.ResultsValid() && !depth_dose.IsEmpty()) {
            ShowResultIndicators(results_tissue, name);
        }
    }
    catch (std::exception& exc) {
        qDebug() << "BraggPeakGui::UpdateResults " << exc.what();
        ClearResults();
        DisplayWarning("Échec du calcul des paramètres de courbe.");
    }
}

void BraggPeakGui::SaveToDb() {
    if (!IsIdle()) {
        DisplayWarning("Please save data once the axes have stopped moving");
        return;
    }

    try {
        if (!depth_dose_->ResultsValid()) {
            throw std::runtime_error("No results valid for depth dose curve");
        }

        QDateTime timestamp(QDateTime::currentDateTime());
        MeasurementCurrents currents(timestamp, 0, 0, 0, 0,
                                     depth_dose_->GetMaxCurrentDiode());
        depth_dose_->SetCurrents(currents);
        depth_dose_->SetTimestamp(timestamp);

        DepthDoseTypeDialog dialog(nullptr);
        if (dialog.exec() == QDialog::Accepted) {
            if (dialog.BraggPeakChosen()) {
                BraggPeak braggPeak(*depth_dose_.get(),
                                    parent_->ui()->b_lineEdit_width_tissue->text().toDouble(),
                                    parent_->ui()->b_lineEdit_penumbra_tissue->text().toDouble(),
                                    parent_->ui()->b_lineEdit_parcours_tissue->text().toDouble());
                repo_->SaveBraggPeak(braggPeak);
                DisplayInfo("BraggPeak successfully saved");
            } else if (dialog.SOBPJourChosen()) {
                SOBP sobp(*depth_dose_.get(),
                          10000, 10000, 0.0, "Daily SOBP",
                          parent_->ui()->b_lineEdit_penumbra_tissue->text().toDouble(),
                          parent_->ui()->b_lineEdit_parcours_tissue->text().toDouble(),
                          parent_->ui()->b_lineEdit_mod98_tissue->text().toDouble(),
                          parent_->ui()->b_lineEdit_mod100_tissue->text().toDouble());
                repo_->SaveSOBP(sobp);
                DisplayInfo("SOBP du jour successfully saved");
            } else if (dialog.SOBPPatientChosen()) {
                SOBP sobp(*depth_dose_.get(),
                          dialog.dossier(), dialog.modulateur(), dialog.degradeur_mm(), dialog.comment(),
                          parent_->ui()->b_lineEdit_penumbra_tissue->text().toDouble(),
                          parent_->ui()->b_lineEdit_parcours_tissue->text().toDouble(),
                          parent_->ui()->b_lineEdit_mod98_tissue->text().toDouble(),
                          parent_->ui()->b_lineEdit_mod100_tissue->text().toDouble());
                repo_->SaveSOBP(sobp);
                DisplayInfo("SOBP patient successfully saved");
            } else {
                throw std::runtime_error("No depth dose curve type was chosen");
            }
        } else {
            qDebug() << "BraggPeakGui::SaveToDb Cancelled by user";
        }
    }
    catch (std::exception& exc) {
        qWarning() << "BraggPeakGui::SaveToDb Exception caught: " << exc.what();
        DisplayError(QString("Failed saving results to db:\n\n") + exc.what());
    }
}

void BraggPeakGui::TabWidgetPageChanged(int index) {
    UpdateResultsPlot();

    TABWIDGETPAGE page = static_cast<TABWIDGETPAGE>(index);
    QGroupBox * box = parent_->ui()->groupBox_b_dateInterval;
    switch (page) {
    case TABWIDGETPAGE::ACQUISITION:
        box->setVisible(false);
        box->setEnabled(false);
        break;
    default:
        box->setVisible(true);
        box->setEnabled(true);
        break;
    }
}

void BraggPeakGui::ReadSignalNoise() {
    qDebug() << "BraggPeakGui::ReadSignalNoise";
    if (!IsIdle()) {
        DisplayWarning("Please wait until axes have stopped before reading noise");
        return;
    }

    try {
        const double noise = dev_factory_->GetSensor(SensorType::DIODE)->InterpretedValue(20);
        depth_dose_->SetSignalNoise(noise);
        parent_->ui()->b_lineEdit_signal_noise->setText(QString::number(noise, 'f', 2));
        DisplayBraggPeak();
    }
    catch (std::exception& exc) {
        qDebug() << "BraggPeakGui::ReadSignalNoise Exception caught " << exc.what();
        DisplayError(QString("Failed reading noise:\n\n") + exc.what());
    }
}

void BraggPeakGui::SetupTimer() {
    timer_.setSingleShot(true);
    timer_.setInterval(1000000);
    QObject::connect(&timer_, &QTimer::timeout, this, [=]() { abort_beacon_.Activate(); abort_beacon_.BroadcastedMessage("BraggPeakGui timeout");});
}

void BraggPeakGui::TurnOffConnectedButton() {
    parent_->ui()->b_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_red_button.png"));
}

void BraggPeakGui::TurnOnConnectedButton() {
    parent_->ui()->b_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_green_button.png"));
}

void BraggPeakGui::LaunchManualMotorControlDialog(Axis axis, std::shared_ptr<IMotor> motor) {
    ManualMotorControlDialog dialog(nullptr, axis, motor);
    dialog.exec();
}

void BraggPeakGui::UploadSOBP() {
    qDebug() << "BraggPeakGui::UploadSOBP";
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before uploading sobps");
        return;
    }

    try {
        FetchSOBPDialog dialog(parent_, repo_);
        if (dialog.exec() != QDialog::Accepted) {
            qDebug() << "BraggPeakGui::FetchToDb User aborted save to db";
            return;
        }
        braggPeak_graph_->ShowLegend(true);
        if (!depth_dose_->IsEmpty()) {
            braggPeak_graph_->SetCurves(measured_graph_name_, depth_dose_->ScaleIntensity(100.0));
        }
        SOBP sobp = dialog.sobp();
        QString name = QString::number(static_cast<int>(uploaded_graph_names_.size()) + 1) +
                ": " + QString::number(sobp.modulateur_id()) + " " + QString::number(sobp.mm_degradeur(), 'f', 1);
        uploaded_graph_names_.push_back(name);
        braggPeak_graph_->InitCurve(name, Axis::Z, 1, Qt::SolidLine);
        braggPeak_graph_->SetCurves(name, sobp.depth_dose().ScaleIntensity(100.0));
//        UpdateResults(name, sobp.depth_dose()); // TODO only for testing
        SOBPResultsDialog results_dialog(nullptr, sobp);
        results_dialog.exec();
    }
    catch (std::exception& exc) {
        ClearUploadedSOBP();
        qDebug() << "SOBPGui::UpdateComparisonGraph " << exc.what();
    }
}

void BraggPeakGui::ShowResultIndicators(const DepthDoseResults& results, QString curve) {
    qDebug() << "BraggPeakGui::ShowResultIndicators " << curve << " IsBraggPeak " << results.IsBraggPeak();
    try {
        if (results.IsBraggPeak()) {
            braggPeak_graph_->ShowHorisontalDistanceToZero(curve, results.parcours(), QString("parcours"), 0, -15);
            braggPeak_graph_->ShowHorisontalDistance(curve, results.penumbraleft(), results.penumbraright(), QString("penumbra"), 0, 15);
            braggPeak_graph_->ShowHorisontalDistance(curve, results.width50left(), results.width50right(), QString("width"), 0, 15);
        } else {
            braggPeak_graph_->ShowHorisontalDistance(curve, results.mod100left(), results.mod100right(), QString("modulation"), -50, -15);
            braggPeak_graph_->ShowHorisontalDistanceToZero(curve, results.parcours(), QString("parcours"), 0, -15);
            braggPeak_graph_->ShowHorisontalDistance(curve, results.penumbraleft(), results.penumbraright(), QString("penumbra"), 0, 15);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "BraggPeakGui::ShowSOBPResultIndicators Exception caught: " << exc.what();
    }
}

void BraggPeakGui::Normalize() {
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before normalizing");
        return;
    }

    try {
        if (!depth_dose_->IsEmpty()) {
            braggPeak_graph_->SetCurves(measured_graph_name_, depth_dose_->ScaleIntensity(parent_->ui()->b_spinBox_normalize->value()));
            braggPeak_graph_->RemoveIndicatorPoints(measured_graph_name_);
            if (depth_dose_->ResultsValid()) {
                ShowResultIndicators(depth_dose_->GetResults(), measured_graph_name_);
            }
        }
    }
    catch (std::exception& exc) {
        qDebug() << "SOBPGui::Normalize " << exc.what();
    }
}

void BraggPeakGui::ClearUploadedSOBP() {
    braggPeak_graph_->ShowLegend(false);
    for (auto name : uploaded_graph_names_) {
        braggPeak_graph_->Remove(name);
    }
    uploaded_graph_names_.clear();
}
