#include "BeamProfilerGui.h"
#include "ui_MQA.h"
#include "MQA.h"

#include <QMessageBox>
#include <QDebug>

#include "ManualMotorControlDialog.h"
#include "Axis.h"
#include "Hardware.h"

BeamProfilerGui::BeamProfilerGui(MQA *parent, std::shared_ptr<QARepo> repo, QA_HARDWARE hardware) :
    GuiBase(parent),
    repo_(repo),
    history_page_(parent, repo),
    hardware_(hardware)
{
    qDebug() << "BeamProfilerGui";
    dev_factory_ = std::shared_ptr<DevFactory>(new DevFactory(repo_));
    profiler_sm_ = std::shared_ptr<BeamProfiler>(new BeamProfiler(dev_factory_, &abort_beacon_, hardware_));
    SetupGraphs();
    SetupBeamProfiles();
    ConnectSignals();
    SetDefaults(); // Must come after connect signals!
    SetupTimer();
    SetupStateMachine();
    parent_->ui()->groupBox_dateInterval->setVisible(false);
    parent_->ui()->groupBox_dateInterval->setEnabled(false);
}

BeamProfilerGui::~BeamProfilerGui() {
    qDebug() << "~BeamProfilerGui";
    TurnOffConnectedButton();
}

void BeamProfilerGui::SetDefaults() {
    parent_->ui()->p_lineEdit_signal_noise->setText("0.0");
    parent_->ui()->p_lineEdit_x_pos->setText("-");
    parent_->ui()->p_lineEdit_y_pos->setText("-");
    parent_->ui()->p_lineEdit_z_pos->setText("-");
    parent_->ui()->p_lineEdit_profileX_centre->setText("-");
    parent_->ui()->p_lineEdit_profileX_flatness->setText("-");
    parent_->ui()->p_lineEdit_profileX_l50->setText("-");
    parent_->ui()->p_lineEdit_profileX_l90->setText("-");
    parent_->ui()->p_lineEdit_profileX_l95->setText("-");
    parent_->ui()->p_lineEdit_profileX_penumbra->setText("-");
    parent_->ui()->p_lineEdit_profileY_centre->setText("-");
    parent_->ui()->p_lineEdit_profileY_flatness->setText("-");
    parent_->ui()->p_lineEdit_profileY_l50->setText("-");
    parent_->ui()->p_lineEdit_profileY_l90->setText("-");
    parent_->ui()->p_lineEdit_profileY_l95->setText("-");
    parent_->ui()->p_lineEdit_profileY_penumbra->setText("-");
}

void BeamProfilerGui::SetRanges() {
    QString prefix;
    switch(hardware_) {
    case QA_HARDWARE::SCANNER2D:
        prefix = "scanner2D/";
        parent_->ui()->p_doubleSpinBox_profileX_step->setValue(GetSetting(QString("x_step").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileX_range_lower->setValue(GetSetting(QString("x_min").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileX_range_upper->setValue(GetSetting(QString("x_max").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileY_step->setValue(GetSetting(QString("y_step").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileY_range_lower->setValue(GetSetting(QString("y_min").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileY_range_upper->setValue(GetSetting(QString("y_max").prepend(prefix)).toDouble());
    case QA_HARDWARE::SCANNER3D:
        prefix = "scanner3D/";
        parent_->ui()->p_doubleSpinBox_profileX_step->setValue(GetSetting(QString("x_step").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileX_range_lower->setValue(GetSetting(QString("x_min").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileX_range_upper->setValue(GetSetting(QString("x_max").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileY_step->setValue(GetSetting(QString("y_step").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileY_range_lower->setValue(GetSetting(QString("y_min").prepend(prefix)).toDouble());
        parent_->ui()->p_doubleSpinBox_profileY_range_upper->setValue(GetSetting(QString("y_max").prepend(prefix)).toDouble());
        break;
    default:
        qWarning() << "BeamProfilerGui::SetRanges non valid hardware";
        parent_->ui()->p_doubleSpinBox_profileX_step->setValue(0);
        parent_->ui()->p_doubleSpinBox_profileX_range_lower->setValue(0);
        parent_->ui()->p_doubleSpinBox_profileX_range_upper->setValue(0);
        parent_->ui()->p_doubleSpinBox_profileY_step->setValue(0);
        parent_->ui()->p_doubleSpinBox_profileY_range_lower->setValue(0);
        parent_->ui()->p_doubleSpinBox_profileY_range_upper->setValue(0);
        break;
    }

}

void BeamProfilerGui::SetupGraphs() {
    parent_->ui()->p_label_chambre_voltage->setAlignment(Qt::AlignCenter);
    parent_->ui()->p_label_diode_voltage->setAlignment(Qt::AlignCenter);

    profile_graph_ = std::shared_ptr<LinePlot>(new LinePlot(parent_->ui()->p_customPlot_Profile));
    profile_graph_->InitCurve(AxisToString(Axis::X), Axis::X, 1, Qt::SolidLine);
    profile_graph_->InitCurve(AxisToString(Axis::Y), Axis::Y, 1, Qt::SolidLine);
    profile_graph_->SetRange(-20.0, 20.0, 0.0, 0.1);
}

void BeamProfilerGui::SetupBeamProfiles() {
    beam_profiles_[Axis::X] = std::shared_ptr<BeamProfile>(new BeamProfile(hardware_, Axis::X));
    beam_profiles_[Axis::Y] = std::shared_ptr<BeamProfile>(new BeamProfile(hardware_, Axis::Y));
}

void BeamProfilerGui::ConnectSignals() {
    QObject::connect(parent_->ui()->p_pushButton_connected, &QPushButton::clicked, this, &BeamProfilerGui::Configure);
    QObject::connect(parent_->ui()->p_pushButton_save, &QPushButton::clicked, this, &BeamProfilerGui::SaveToDb);

    QObject::connect(&abort_beacon_, &Beacon::BroadcastedMessage, this, &BeamProfilerGui::DisplayError);
    QObject::connect(parent_->ui()->p_pushButton_normalize, &QPushButton::clicked, this, &BeamProfilerGui::Normalize);
    QObject::connect(parent_->ui()->p_pushButton_profileX_show, &QPushButton::clicked, this, [=]() { ToggleVisibleButton(Axis::X); });
    QObject::connect(parent_->ui()->p_pushButton_profileY_show, &QPushButton::clicked, this, [=]() { ToggleVisibleButton(Axis::Y); });
    QObject::connect(parent_->ui()->p_tabWidget, &QTabWidget::currentChanged, this, [=](int index) {
        QGroupBox * box = parent_->ui()->groupBox_dateInterval;
        if (index == 1) { box->setVisible(true); box->setEnabled(true); }
        else {  box->setVisible(false); box->setEnabled(false); }});

    // tabwidget
    QObject::connect(parent_->ui()->p_tabWidget, &QTabWidget::currentChanged, this, &BeamProfilerGui::TabWidgetPageChanged);

    // Signal noise
    QObject::connect(parent_->ui()->p_pushButton_read_signal_noise, &QPushButton::pressed, this, &BeamProfilerGui::ReadSignalNoise);
    QObject::connect(this, &BeamProfilerGui::SIGNAL_AxisDeviation, this, [&](Axis axis, double value) {
        if (axis == Axis::X) parent_->ui()->p_lineEdit_x_diff->setText(QString::number(value, 'f', 2));
        if (axis == Axis::Y) parent_->ui()->p_lineEdit_y_diff->setText(QString::number(value, 'f', 2));
        if (axis == Axis::Z) parent_->ui()->p_lineEdit_z_diff->setText(QString::number(value, 'f', 2));
    });

    // Profile Acquisition
    QObject::connect(parent_->ui()->p_pushButton_profileX_autocenter, &QPushButton::clicked, this, [&]() {
        if (hardware_ == QA_HARDWARE::SCANNER3D) {
            DisplayModelessMessageBox("Autocenter not allowed with the water tank", false, 0, QMessageBox::Warning);
        } else {
            StartAutoCenter(Axis::X);
        }
    });
    QObject::connect(parent_->ui()->p_pushButton_profileY_autocenter, &QPushButton::clicked, this, [&]() {
        if (hardware_ == QA_HARDWARE::SCANNER3D) {
            DisplayModelessMessageBox("Autocenter not allowed with the water tank", false, 0, QMessageBox::Warning);
        } else {
            StartAutoCenter(Axis::Y);
        }
    });
    QObject::connect(parent_->ui()->p_pushButton_start, &QPushButton::clicked, this, &BeamProfilerGui::StartAcquisitionOnBothAxes);
    QObject::connect(parent_->ui()->p_pushButton_abort, &QPushButton::clicked, this, &BeamProfilerGui::AbortAcquisition);
    QObject::connect(parent_->ui()->p_pushButton_movetozero, &QPushButton::clicked, this,  &BeamProfilerGui::MovingToZero);
    QObject::connect(profiler_sm_.get(), &BeamProfiler::ScanFinished, this, &BeamProfilerGui::AutoCenter);
    QObject::connect(profiler_sm_.get(), &BeamProfiler::ScanFinished, this, &BeamProfilerGui::DisplayBeamProfile);
    QObject::connect(profiler_sm_.get(), &BeamProfiler::ScanFinished, this, [&]() { if (hardware_ == QA_HARDWARE::SCANNER3D) VerifyAxesCalibrations(); });
    QObject::connect(parent_->ui()->p_pushButton_profileX_start, &QPushButton::clicked, this, [=]() { StartAcquisition(Axis::X); });
    QObject::connect(parent_->ui()->p_pushButton_profileY_start, &QPushButton::clicked, this, [=]() { StartAcquisition(Axis::Y); });

    QObject::connect(profiler_sm_.get(), &BeamProfiler::Measurement, this, &BeamProfilerGui::AddDataPointToContainer);
    QObject::connect(profiler_sm_.get(), &BeamProfiler::Measurement, this, &BeamProfilerGui::AddDataPointToGraph);

    QObject::connect(profiler_sm_.get(), &BeamProfiler::Measurement, this, [=](Axis axis, MeasurementPoint p) {
        (void)axis; parent_->ui()->p_label_chambre_voltage->setText(QString::number(abs(p.signal().reference()), 'f', 2));
    });
    QObject::connect(profiler_sm_.get(), &BeamProfiler::Measurement, this, [=](Axis axis, MeasurementPoint p) {
        (void)axis; parent_->ui()->p_label_diode_voltage->setText(QString::number(abs(p.signal().raw()), 'f', 2));
    });
    QObject::connect(profiler_sm_.get(), &BeamProfiler::SIGNAL_DiodeVoltage, this, [&](double value) {
        parent_->ui()->p_label_diode_voltage->setText(QString::number(abs(value), 'f', 2));
    });
    QObject::connect(profiler_sm_.get(), &BeamProfiler::SIGNAL_ChambreVoltage, this, [&](double value) {
        parent_->ui()->p_label_chambre_voltage->setText(QString::number(abs(value), 'f', 2));
    });

    // Ranges
    QObject::connect(parent_->ui()->p_doubleSpinBox_profileX_range_lower, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, [=](double d) { LowerScanRangeChanged(Axis::X, d); });
    QObject::connect(parent_->ui()->p_doubleSpinBox_profileX_range_upper, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, [=](double d) { UpperScanRangeChanged(Axis::X, d); });
    QObject::connect(parent_->ui()->p_doubleSpinBox_profileY_range_lower, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, [=](double d) { LowerScanRangeChanged(Axis::Y, d); });
    QObject::connect(parent_->ui()->p_doubleSpinBox_profileY_range_upper, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, [=](double d) { UpperScanRangeChanged(Axis::Y, d); });
}

void BeamProfilerGui::AddDataPointToContainer(Axis axis, MeasurementPoint m) {
    *beam_profiles_[axis].get() << m;
}

void BeamProfilerGui::AddDataPointToGraph(Axis axis, MeasurementPoint m) {
    if (++graph_update_counter_ == 10) {
        graph_update_counter_ = 0;
        profile_graph_->AddPoint(AxisToString(axis), m, beam_profiles_[axis]->GetSignalNoise());
    }
}


void BeamProfilerGui::Configure() {
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before reconfiguring hardware");
        return;
    }

    try {
        SetRanges();
        TurnOffConnectedButton();
        parent_->ui()->p_pushButton_manual_control_z->setEnabled(hardware_ == QA_HARDWARE::SCANNER3D);
        dev_factory_->Setup(hardware_);
        QObject::disconnect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition, this, 0);
        QObject::connect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition,
                         this, [&](double pos) { parent_->ui()->p_lineEdit_x_pos->setText(QString::number(pos, 'f', 2)); });
        QObject::connect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition,
                         this, [&](double pos) { parent_->ui()->p_lineEdit_y_pos->setText(QString::number(pos, 'f', 2)); });

        QObject::disconnect(parent_->ui()->p_pushButton_manual_control_x, 0, 0, 0);
        QObject::disconnect(parent_->ui()->p_pushButton_manual_control_y, 0, 0, 0);
        QObject::connect(parent_->ui()->p_pushButton_manual_control_x, &QPushButton::clicked, this, [=](){
            LaunchManualMotorControlDialog(Axis::X, dev_factory_->GetMotor(Axis::X)); });
        QObject::connect(parent_->ui()->p_pushButton_manual_control_y, &QPushButton::clicked, this, [=](){
            LaunchManualMotorControlDialog(Axis::Y, dev_factory_->GetMotor(Axis::Y)); });

        TurnOnConnectedButton();
        parent_->ui()->p_lineEdit_x_pos->setText(QString::number(dev_factory_->GetMotor(Axis::X)->ActPos(), 'f', 2));
        parent_->ui()->p_lineEdit_y_pos->setText(QString::number(dev_factory_->GetMotor(Axis::Y)->ActPos(), 'f', 2));

        if (hardware_ == QA_HARDWARE::SCANNER3D) {
            QObject::disconnect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition, this, 0);
            QObject::disconnect(parent_->ui()->p_pushButton_manual_control_z, 0, 0, 0);
            QObject::connect(parent_->ui()->p_pushButton_manual_control_z, &QPushButton::clicked, this, [=](){
                LaunchManualMotorControlDialog(Axis::Z, dev_factory_->GetMotor(Axis::Z)); });
            QObject::connect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition,
                             this, [&](double pos) { parent_->ui()->p_lineEdit_z_pos->setText(QString::number(pos, 'f', 2)); });
            parent_->ui()->p_lineEdit_z_pos->setText(QString::number(dev_factory_->GetMotor(Axis::Z)->ActPos(), 'f', 2));
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

void BeamProfilerGui::VerifyAxisCalibration(Axis axis) {
    try {
        double diff;
        bool ok = dev_factory_->VerifyAxisPositionCalibration(axis, diff);
        qDebug() << "BeamProfilerGui::VerifyAxisCalibration Axis " << AxisToString(axis) << " " << diff;
        emit SIGNAL_AxisDeviation(axis, diff);

        if (!ok) {
            QString message = AxisToString(axis) + "-axis has a larger position deviation than allowed. Consider recalibrating axis.\n";
            message.append("\ndelta = ").append(QString::number(diff, 'f', 2));
            DisplayModelessMessageBox(message, false, 0, QMessageBox::Warning);
        }
        qDebug() << "BeamProfilerGui::VerifyAxisCalibration " << AxisToString(axis) << " delta = " << diff;
    } catch (std::exception& exc) {
        QString msg = QString("Failed verifying axis calibrations: ") + exc.what();
        DisplayCritical(msg);
        return;
    }
}

bool BeamProfilerGui::VerifyAxesCalibrations() {
    try {
        double x_diff, y_diff, z_diff;
        bool x_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::X, x_diff);
        bool y_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::Y, y_diff);
        bool z_ok = dev_factory_->VerifyAxisPositionCalibration(Axis::Y, z_diff);
        qDebug() << "BeamProfilerGui::VerifyAxesCalibrations (X, Y, Z) " << x_diff << " " << y_diff << " " << z_diff;

        emit SIGNAL_AxisDeviation(Axis::X, x_diff);
        emit SIGNAL_AxisDeviation(Axis::Y, y_diff);
        emit SIGNAL_AxisDeviation(Axis::Z, z_diff);

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

void BeamProfilerGui::CleanUp() {
    try {
        qDebug() << "BeamProfilerGui::CleanUp";
        emit profiler_sm_ ->StopStateMachine();
        dev_factory_->CleanUp();
    }
   catch (std::exception& exc) {
       qDebug() << "BeamProfilerGui::CleanUp Exception caught " << exc.what();
        DisplayError(QString("Failed cleaning up hardware:\n\n") + exc.what());
   }
}

bool BeamProfilerGui::IsIdle() const {
    return profiler_sm_->IsIdle() || profiler_sm_->IsFinished();
}


void BeamProfilerGui::LowerScanRangeChanged(Axis axis, double value) {
    (void)axis;
    (void)value;
    double min = std::min(parent_->ui()->p_doubleSpinBox_profileX_range_lower->value(),
                          parent_->ui()->p_doubleSpinBox_profileY_range_lower->value());
    profile_graph_->SetHorLowerRange(-1.0 + min);
}

void BeamProfilerGui::UpperScanRangeChanged(Axis axis, double value) {
    (void)axis;
    (void)value;
    double max = std::max(parent_->ui()->p_doubleSpinBox_profileX_range_upper->value(),
                          parent_->ui()->p_doubleSpinBox_profileY_range_upper->value());
    profile_graph_->SetHorUpperRange(1.0 + max);
}

Range BeamProfilerGui::GetRange(Axis axis) const {
    double max(0.0);
    double min(0.0);
    if (axis == Axis::X) {
        max = parent_->ui()->p_doubleSpinBox_profileX_range_upper->value();
        min = parent_->ui()->p_doubleSpinBox_profileX_range_lower->value();
    } else if (axis == Axis::Y) {
        max = parent_->ui()->p_doubleSpinBox_profileY_range_upper->value();
        min = parent_->ui()->p_doubleSpinBox_profileY_range_lower->value();
    } else {
        throw std::runtime_error("Wrong axis, range is only given for the X and Y axis");
    }
    return Range(min, max);
}

double BeamProfilerGui::GetStep(Axis axis) const {
    double step(0.0);
    if (axis == Axis::X) {
        step = parent_->ui()->p_doubleSpinBox_profileX_step->value();
    } else if (axis == Axis::Y) {
        step = parent_->ui()->p_doubleSpinBox_profileY_step->value();
    } else {
        throw std::runtime_error("Wrong axis, range is only given for the X and Y axis");
    }
    return step;
}

double BeamProfilerGui::GetNmbMeasurements() const {
    return parent_->ui()->p_spinBox_n_measurements->value();
}

double BeamProfilerGui::GetMaxStdError() const {
    return parent_->ui()->p_spinBox_sem->value();
}

bool BeamProfilerGui::GetFixedNmbMeasurements()  const {
    return parent_->ui()->p_radioButton_enable_fixed_nmb_measurements->isChecked();
}

void BeamProfilerGui::SetupStateMachine() {
       QState *sSuperState = new QState();
       QState *sIdling = new QState(sSuperState);
       QState *sProfilingX = new QState(sSuperState);
       QState *sProfilingY = new QState(sSuperState);

       QObject::connect(sIdling, &QState::entered, this, [=]() { StateMachineMessage("Idling entered"); });
       QObject::connect(sIdling, &QState::exited, this, [=]() { StateMachineMessage("Idling exited"); });
       QObject::connect(sProfilingX, &QState::entered, this, [=]() { StateMachineMessage("ProfilingX entered"); });
       QObject::connect(sProfilingX, &QState::exited, this, [=]() { StateMachineMessage("ProfilingX exited"); });
       QObject::connect(sProfilingY, &QState::entered, this, [=]() { StateMachineMessage("ProfilingY entered"); });
       QObject::connect(sProfilingY, &QState::exited, this, [=]() { StateMachineMessage("ProfilingY exited"); });

       // SuperState
       sSuperState->setInitialState(sIdling);
       sSuperState->addTransition(&abort_beacon_, SIGNAL(Activated()), sSuperState);

       // Idling state
       sIdling->addTransition(this, SIGNAL(StartAcquisitionOnBothAxes()), sProfilingX);

       // ProfilingX state
       QObject::connect(sProfilingX, &QState::entered, this, [=]() { StartAcquisition(Axis::X); });
       QObject::connect(sProfilingX, &QState::entered, this, [=](){ timer_.start(1000000); });
       QObject::connect(sProfilingX, &QState::exited, this, [=](){ timer_.stop(); });
       sProfilingX->addTransition(profiler_sm_.get(), &BeamProfiler::AxisFinished, sProfilingY);

       // ProfilingY state
       QObject::connect(sProfilingY, &QState::entered, this, [=]() { StartAcquisition(Axis::Y); });
       QObject::connect(sProfilingY, &QState::entered, this, [=](){ timer_.start(1000000); });
       QObject::connect(sProfilingY, &QState::exited, this, [=](){ timer_.stop(); });
       sProfilingY->addTransition(profiler_sm_.get(),  &BeamProfiler::AxisIdle, sIdling);

       sm_.addState(sSuperState);
       sm_.setInitialState(sSuperState);
       sm_.start();
}

void BeamProfilerGui::StartAcquisition(Axis axis) {
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before starting a new measurement");
        return;
    }
    if (hardware_ == QA_HARDWARE::SCANNER3D) {
        if (!VerifyAxesCalibrations()) {
            return;
        }
    }
    ClearAll(axis);
    profiler_sm_->Start(axis, GetRange(axis), GetStep(axis), GetNmbMeasurements(),
                        GetMaxStdError(), GetFixedNmbMeasurements());
}

void BeamProfilerGui::StartAutoCenter(Axis axis) {
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before starting an auto center");
        return;
    }
    if (hardware_ == QA_HARDWARE::SCANNER3D) {
        if (!VerifyAxesCalibrations()) {
            return;
        }
    }
    auto_center_ = true;
    StartAcquisition(axis);
}

void BeamProfilerGui::AbortAcquisition() {
    qDebug() << "BeamProfilerGui::AbortAcquisition";
    auto_center_ = false;
    abort_beacon_.Activate();
    DisplayBeamProfile(profiler_sm_->GetActiveAxis());
}

void BeamProfilerGui::ClearAll(Axis axis) {
    beam_profiles_[axis]->Clear();
    ClearGraphs(axis);
    ClearResults(axis);
}

void BeamProfilerGui::ClearGraphs(Axis axis) {
    try {
        profile_graph_->Clear(AxisToString(axis));
    }
    catch (std::exception& exc) {
        qWarning() << "BeamProfilerGui::ClearGraphs Exception thrown: " << exc.what();
    }
}

void BeamProfilerGui::ClearResults(Axis axis) {
    switch(axis) {
    case Axis::X:
        parent_->ui()->p_lineEdit_profileX_centre->setText("NA");
        parent_->ui()->p_lineEdit_profileX_l50->setText("NA");
        parent_->ui()->p_lineEdit_profileX_l90->setText("NA");
        parent_->ui()->p_lineEdit_profileX_l95->setText("NA");
        parent_->ui()->p_lineEdit_profileX_penumbra->setText("NA");
        parent_->ui()->p_lineEdit_profileX_flatness->setText("NA");
        break;
    case Axis::Y:
        parent_->ui()->p_lineEdit_profileY_centre->setText("NA");
        parent_->ui()->p_lineEdit_profileY_l50->setText("NA");
        parent_->ui()->p_lineEdit_profileY_l90->setText("NA");
        parent_->ui()->p_lineEdit_profileY_l95->setText("NA");
        parent_->ui()->p_lineEdit_profileY_penumbra->setText("NA");
        parent_->ui()->p_lineEdit_profileY_flatness->setText("NA");
        break;
    default:
        qWarning() << "BeamProfilerGui::ClearResults Unknown axis";
    }
}

void BeamProfilerGui::DisplayBeamProfile(Axis axis) {
    if (axis != Axis::UNK) {
        UpdateGraphs(axis);
        UpdateResults(axis);
    } else {
        qDebug() << "BeamProfilerGui::DisplayBeamProfile Unknown axis";
    }
}

void BeamProfilerGui::UpdateGraphs(Axis axis) {
    try {
        profile_graph_->SetCurves(AxisToString(axis), *beam_profiles_[axis].get());
    }
    catch (std::exception& exc) {
        profile_graph_->Clear(AxisToString(axis));
        qDebug() << "BeamProfilerGui::UpdateGraphs " << exc.what();
        DisplayError(QString("Failed updating beam profile graph"));
    }
}

void BeamProfilerGui::UpdateResults(Axis axis) {
    try {
        BeamProfileResults results = beam_profiles_[axis]->GetResults();
        switch (axis) {
        case Axis::X:
            parent_->ui()->p_lineEdit_profileX_centre->setText(QString::number(results.centre(), 'f', 2));
            parent_->ui()->p_lineEdit_profileX_l50->setText(QString::number(results.width_50(), 'f', 2));
            parent_->ui()->p_lineEdit_profileX_l90->setText(QString::number(results.width_90(), 'f', 2));
            parent_->ui()->p_lineEdit_profileX_l95->setText(QString::number(results.width_95(), 'f', 2));
            parent_->ui()->p_lineEdit_profileX_penumbra->setText(QString::number(results.penumbra(), 'f', 2));
            parent_->ui()->p_lineEdit_profileX_flatness->setText(QString::number(results.flatness(), 'f', 2));
            break;
        case Axis::Y:
            parent_->ui()->p_lineEdit_profileY_centre->setText(QString::number(results.centre(), 'f', 2));
            parent_->ui()->p_lineEdit_profileY_l50->setText(QString::number(results.width_50(), 'f', 2));
            parent_->ui()->p_lineEdit_profileY_l90->setText(QString::number(results.width_90(), 'f', 2));
            parent_->ui()->p_lineEdit_profileY_l95->setText(QString::number(results.width_95(), 'f', 2));
            parent_->ui()->p_lineEdit_profileY_penumbra->setText(QString::number(results.penumbra(), 'f', 2));
            parent_->ui()->p_lineEdit_profileY_flatness->setText(QString::number(results.flatness(), 'f', 2));
            break;
        default:
            qWarning() << "BeamProfilerGui::UpdateResults Unknown case";
            break;
        }
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfilerGui::UpdateResults " << exc.what();
        switch (axis) {
        case Axis::X:
            parent_->ui()->p_lineEdit_profileX_centre->setText("NA");
            parent_->ui()->p_lineEdit_profileX_l50->setText("NA");
            parent_->ui()->p_lineEdit_profileX_l90->setText("NA");
            parent_->ui()->p_lineEdit_profileX_l95->setText("NA");
            parent_->ui()->p_lineEdit_profileX_penumbra->setText("NA");
            parent_->ui()->p_lineEdit_profileX_flatness->setText("NA");
            break;
        case Axis::Y:
            parent_->ui()->p_lineEdit_profileY_centre->setText("NA");
            parent_->ui()->p_lineEdit_profileY_l50->setText("NA");
            parent_->ui()->p_lineEdit_profileY_l90->setText("NA");
            parent_->ui()->p_lineEdit_profileY_l95->setText("NA");
            parent_->ui()->p_lineEdit_profileY_penumbra->setText("NA");
            parent_->ui()->p_lineEdit_profileY_flatness->setText("NA");
            break;
        default:
            qWarning() << "BeamProfilerGui::UpdateResults Unknown case";
            break;
        }
        qWarning() << "BeamProfilerGui::UpdateResults exception " << exc.what();
    }
}

void BeamProfilerGui::SaveToDb() {
    if (!IsIdle()) {
        DisplayWarning("Please save data once the axes have stopped moving");
        return;
    }

    try {
        if (!beam_profiles_[Axis::X]->ResultsValid()) {
            throw std::runtime_error("No results available for X profile");
        }
        if (!beam_profiles_[Axis::Y]->ResultsValid()) {
            throw std::runtime_error("No results available for Y profile");
        }

        QDateTime time(QDateTime::currentDateTime());
        MeasurementCurrents currents_X(time, 0, 0, 0, 0,
                                     beam_profiles_[Axis::X]->GetMaxCurrentDiode());
        MeasurementCurrents currents_Y(time, 0, 0, 0, 0,
                                     beam_profiles_[Axis::Y]->GetMaxCurrentDiode());
        beam_profiles_[Axis::X]->SetCurrents(currents_X);
        beam_profiles_[Axis::Y]->SetCurrents(currents_Y);
        beam_profiles_[Axis::X]->SetTimestamp(time);
        beam_profiles_[Axis::Y]->SetTimestamp(time);
        repo_->SaveBeamProfile(*beam_profiles_[Axis::X].get());
        repo_->SaveBeamProfile(*beam_profiles_[Axis::Y].get());
        DisplayInfo("Results successfully saved");
    }
    catch (std::exception& exc) {
        qWarning() << "BeamProfilerGui::SaveToDb Exception caught: " << exc.what();
        DisplayError(QString("Failed saving profiles to db:\n\n") + exc.what());
    }
}

void BeamProfilerGui::TabWidgetPageChanged(int index) {
    switch (static_cast<TABWIDGETPAGE>(index)) {
    case TABWIDGETPAGE::ACQUISITION:
        break;
    case TABWIDGETPAGE::RESULTS:
        history_page_.FetchData();
        break;
    default:
        qWarning() << "BeamProfilerGui:TabWidgetPageChanged Unknown index " << index;
    }
}

void BeamProfilerGui::MovingToZero() {
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before moving to start position");
        return;
    }
    if (hardware_ == QA_HARDWARE::SCANNER3D) {
        if (!VerifyAxesCalibrations()) {
            return;
        }
    }
    emit profiler_sm_->MovingToZero();
}


void BeamProfilerGui::ReadSignalNoise() {
    qDebug() << "BeamProfilerGui::ReadSignalNoise";
    parent_->ui()->p_lineEdit_signal_noise->setText("0");
    if (!IsIdle()) {
        DisplayWarning("Please wait until axes have stopped before reading noise");
        return;
    }

    try {
        auto diode = dev_factory_->GetSensor(SensorType::DIODE);
        double noise = diode->InterpretedValue(20, 1);
        qDebug() << "Value: " << noise;
        beam_profiles_[Axis::X]->SetSignalNoise(noise);
        beam_profiles_[Axis::Y]->SetSignalNoise(noise);
        parent_->ui()->p_lineEdit_signal_noise->setText(QString::number(noise, 'f', 2));
        DisplayBeamProfile(Axis::X);
        DisplayBeamProfile(Axis::Y);
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfilerGui::ReadSignalNoise Exception caught " << exc.what();
        DisplayError(QString("Failed reading noise:\n\n") + exc.what());
    }
}

void BeamProfilerGui::SetupTimer() {
    timer_.setSingleShot(true);
    timer_.setInterval(1000000);
    QObject::connect(&timer_, &QTimer::timeout, this, [=]() { abort_beacon_.Activate(); emit abort_beacon_.BroadcastedMessage("BeamProfilerGui timeout");});
}

void BeamProfilerGui::ToggleVisibleButton(Axis axis) {
    switch (axis) {
    case Axis::X:
        if (profile_graph_->IsVisible(AxisToString(Axis::X))) {
            profile_graph_->ToggleVisible(AxisToString(Axis::X), false);
            parent_->ui()->p_pushButton_profileX_show->setStyleSheet(ButtonStyleSheet("notvisible_25.png"));
        } else {
            profile_graph_->ToggleVisible(AxisToString(Axis::X), true);
            parent_->ui()->p_pushButton_profileX_show->setStyleSheet(ButtonStyleSheet("visible_25.png"));
        }
        break;
    case Axis::Y:
        if (profile_graph_->IsVisible(AxisToString(Axis::Y))) {
            profile_graph_->ToggleVisible(AxisToString(Axis::Y), false);
            parent_->ui()->p_pushButton_profileY_show->setStyleSheet(ButtonStyleSheet("notvisible_25.png"));
        } else {
            profile_graph_->ToggleVisible(AxisToString(Axis::Y), true);
            parent_->ui()->p_pushButton_profileY_show->setStyleSheet(ButtonStyleSheet("visible_25.png"));
        }
        break;
    default:
        break;
    }
}

void BeamProfilerGui::TurnOffConnectedButton() {
    parent_->ui()->p_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_red_button.png"));
}

void BeamProfilerGui::TurnOnConnectedButton() {
    parent_->ui()->p_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_green_button.png"));
}

void BeamProfilerGui::LaunchManualMotorControlDialog(Axis axis, std::shared_ptr<IMotor> motor) {
    qDebug() << "Launch";
    ManualMotorControlDialog dialog(nullptr, axis, motor);
    dialog.exec();
}

void BeamProfilerGui::AutoCenter(Axis axis) {
    try {
        if (auto_center_ && beam_profiles_[axis]->ResultsValid()) {
            auto_center_ = false;
            const double new_centre = beam_profiles_[axis]->GetResults().centre();
            qDebug() << "BeamProfilerGui::AutoCenter Setting new center position " << QString::number(new_centre, 'f', 3);
            beam_profiles_[axis]->Translate(-new_centre);
            dev_factory_->GetMotor(axis)->SetToZeroPos(new_centre);
        }
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfilerGui::AutoCenter Exception caught " << exc.what();
        DisplayError(QString("Failed setting new center position:\n\n") + exc.what());
    }
    profiler_sm_->Acknowledged();
}

void BeamProfilerGui::Normalize() {
    if (!IsIdle()) {
        DisplayWarning("Please stop axes before normalizing");
        return;
    }

    try {
        if (!beam_profiles_[Axis::X]->IsEmpty()) {
            profile_graph_->SetCurves(AxisToString(Axis::X), beam_profiles_[Axis::X]->ScaleIntensity(parent_->ui()->p_spinBox_normalize->value()));
        }
        if (!beam_profiles_[Axis::Y]->IsEmpty()) {
            profile_graph_->SetCurves(AxisToString(Axis::Y), beam_profiles_[Axis::Y]->ScaleIntensity(parent_->ui()->p_spinBox_normalize->value()));
        }
    }
    catch (std::exception& exc) {
        qDebug() << "BeamProfilerGui::Normalize " << exc.what();
    }
}


