#include "Scanner3DGui.h"
#include "ui_MQA.h"
#include "MQA.h"

#include <QMessageBox>
#include <QDebug>

#include "Axis.h"
#include "SaveScanner3DDialog.h"
#include "FetchScanner3DDialog.h"

#include <iostream>

Scanner3DGui::Scanner3DGui(MQA *parent, std::shared_ptr<QARepo> repo) :
    GuiBase(parent),
    repo_(repo)
{
    dev_factory_ = std::shared_ptr<DevFactory>(new DevFactory(repo_));
    scanner_ = std::shared_ptr<Scanner3D>(new Scanner3D(dev_factory_, &abort_beacon_));
    SetupGraphs();
    ConnectSignals();
    FillComboBoxes();
    SetDefaults();
    SetupValidators();
}

Scanner3DGui::~Scanner3DGui() {}

void Scanner3DGui::FillComboBoxes() {
    parent_->ui()->s3d_comboBox_projection->addItem(QString("X-Y"), static_cast<int>(PROJECTION::XY));
    parent_->ui()->s3d_comboBox_projection->addItem(QString("Y-Z"), static_cast<int>(PROJECTION::YZ));
    parent_->ui()->s3d_comboBox_projection->addItem(QString("Z-X"), static_cast<int>(PROJECTION::ZX));
    parent_->ui()->s3d_comboBox_projection->setCurrentIndex(parent_->ui()->s3d_comboBox_projection->findData(static_cast<int>(PROJECTION::XY)));
}

void Scanner3DGui::SetDefaults() {
    parent_->ui()->s3d_radioButton_interpolate->setChecked(false);
    parent_->ui()->s3d_spinBox_isolevels->setValue(350);
}

void Scanner3DGui::SetupValidators() {
}

void Scanner3DGui::SetupGraphs() {
   projection_graph_ = std::shared_ptr<CuveCustomPlot>(new CuveCustomPlot(parent_->ui()->s3d_customPlot_slice, "X [mm]", "Y [mm]"));
   chambre_histogram_ = std::shared_ptr<Histogram>(new Histogram(parent_->ui()->s3d_customPlot_refsignal_histogram, "", "", 0.0, 2.2, 22));
   diode_histogram_ = std::shared_ptr<Histogram>(new Histogram(parent_->ui()->s3d_customPlot_diode_histogram, "", "", 0.0, 2.2, 22));
   chambre_histogram_->Register("Reference");
   diode_histogram_->Register("Diode");

}

void Scanner3DGui::ConnectSignals() {
    QObject::connect(projection_graph_.get(), &CuveCustomPlot::MouseOver, this, &Scanner3DGui::PrintMouseOver);

    QObject::connect(scanner_.get(), &Scanner3D::Measurement, this, &Scanner3DGui::HandleMeasurement);
    QObject::connect(scanner_.get(), &Scanner3D::Measurement, this, [=](MeasurementPoint p) { chambre_histogram_->SetSingleElement("Reference", p.signal().reference()); });
    QObject::connect(scanner_.get(), &Scanner3D::Measurement, this, [=](MeasurementPoint p) { diode_histogram_->SetSingleElement("Diode", p.signal().raw()); });
    QObject::connect(scanner_.get(), &Scanner3D::Finished, this, &Scanner3DGui::DisplayCube);

    QObject::connect(parent_->ui()->s3d_pushButton_save, &QPushButton::clicked, this, &Scanner3DGui::SaveToDb);
    QObject::connect(parent_->ui()->s3d_pushButton_fetch_from_db, &QPushButton::clicked, this, &Scanner3DGui::FetchFromDb);
    QObject::connect(parent_->ui()->s3d_pushButton_start, &QPushButton::clicked, this, &Scanner3DGui::ClearAll);
    QObject::connect(parent_->ui()->s3d_pushButton_start, &QPushButton::clicked, this, &Scanner3DGui::StartAcquisition);
    QObject::connect(parent_->ui()->s3d_pushButton_abort, &QPushButton::clicked, this, &Scanner3DGui::AbortAcquisition);

    QObject::connect(&abort_beacon_, &Beacon::BroadcastedMessage, this, &Scanner3DGui::DisplayError);

    QObject::connect(parent_->ui()->s3d_pushButton_read_signal_noise, &QPushButton::clicked, this, &Scanner3DGui::ReadSignalNoise);

    QObject::connect(parent_->ui()->s3d_radioButton_interpolate, &QRadioButton::toggled, projection_graph_.get(), &CuveCustomPlot::SetInterpolate);
    QObject::connect(parent_->ui()->s3d_spinBox_isolevels, SIGNAL(valueChanged(int)), projection_graph_.get(), SLOT(SetGradientLevels(int)));

    QObject::connect(parent_->ui()->s3d_comboBox_projection, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                     this, [=]() { LoadProjection(CurrentProjection()); });
    QObject::connect(parent_->ui()->s3d_spinBox_active_slice, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, [=](int value) { LoadSlice(CurrentProjection(), value); });
}

void Scanner3DGui::Configure() {
    try {
        TurnOffConnectedButton();
        dev_factory_->Setup(QA_HARDWARE::SCANNER3D);
        QObject::disconnect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition, this, 0);
        QObject::disconnect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition, this, 0);
        QObject::connect(dev_factory_->GetMotor(Axis::X).get(), &IMotor::CurrentPosition,
                         this, [&](double pos) { parent_->ui()->s3d_lineEdit_x_pos->setText(QString::number(pos, 'f', 2)); });
        QObject::connect(dev_factory_->GetMotor(Axis::Y).get(), &IMotor::CurrentPosition,
                         this, [&](double pos) { parent_->ui()->s3d_lineEdit_y_pos->setText(QString::number(pos, 'f', 2)); });
        QObject::connect(dev_factory_->GetMotor(Axis::Z).get(), &IMotor::CurrentPosition,
                         this, [&](double pos) { parent_->ui()->s3d_lineEdit_z_pos->setText(QString::number(pos, 'f', 2)); });

        TurnOnConnectedButton();
        parent_->ui()->s3d_lineEdit_x_pos->setText(QString::number(dev_factory_->GetMotor(Axis::X)->ActPos(), 'f', 2));
        parent_->ui()->s3d_lineEdit_y_pos->setText(QString::number(dev_factory_->GetMotor(Axis::Y)->ActPos(), 'f', 2));
        parent_->ui()->s3d_lineEdit_z_pos->setText(QString::number(dev_factory_->GetMotor(Axis::Z)->ActPos(), 'f', 2));
    }
    catch (std::exception& exc) {
        TurnOffConnectedButton();
        QString msg = QString("Failed configuring hardware: ") + exc.what();
        DisplayCritical(msg);
    }
}

void Scanner3DGui::StartAcquisition() {
    auto ranges = GetRanges();
    auto step_sizes = GetStepSizes();
    auto n_steps = GetNSteps(ranges, step_sizes);
    auto configs = GetAxisConfigs(n_steps, ranges, step_sizes);
    cube_ = CuveCube(configs, parent_->ui()->s3d_lineEdit_signal_noise->text().toDouble());
    InitProjection(CurrentProjection());
    scanner_->Start(ranges, step_sizes, GetNmbMeasurements(), GetMaxStdError(), GetFixedNmbMeasurements());
}

void Scanner3DGui::AbortAcquisition() {
    abort_beacon_.Activate();
    DisplayCube();
}

void Scanner3DGui::CleanUp() {
    try {
        qDebug() << "Scanner3DGui::CleanUp";
        scanner_->StopStateMachine();
        dev_factory_->CleanUp();
     }
     catch (std::exception& exc) {
         qDebug() << "Scanner3DGui::CleanUp Exception caught " << exc.what();
         DisplayError(QString("Failed cleaning up hardware: ") + exc.what());
   }
}

void Scanner3DGui::HandleMeasurement(MeasurementPoint m) {
    cube_.Add(m);

    const double z_tissue = material::ToTissue(m.pos(Axis::Z), IsMadeOf(QA_HARDWARE::SCANNER3D));
    switch (CurrentProjection()) {
    case PROJECTION::XY:
        projection_graph_->AddPointByCoord(m.pos(Axis::X), m.pos(Axis::Y), m.signal().value(cube_.GetNoise()));
        break;
    case PROJECTION::YZ:
        projection_graph_->AddPointByCoord(m.pos(Axis::Y), z_tissue, m.signal().value(cube_.GetNoise()));
        break;
    case PROJECTION::ZX:
        projection_graph_->AddPointByCoord(z_tissue, m.pos(Axis::X), m.signal().value(cube_.GetNoise()));
        break;
    default:
        break;
    }
    projection_graph_->Rescale();
}

void Scanner3DGui::DisplayCube() {
    qDebug() << "Scanner3DGui::DisplayCube";
    LoadProjection(CurrentProjection());
    UpdateGraphs();
}

void Scanner3DGui::InitProjection(PROJECTION proj) {
    qDebug() << "Scanner3DGui::InitProjection " << static_cast<int>(proj);
    projection_graph_->Clear();

    Axis depth_axis = DepthAxis(proj);
    const int n_slices = cube_.nbins(depth_axis);
    if (n_slices == 0) {
        qWarning() << "Scanner3DGui::InitProjection Nslices = 0";
        return;
    }
    parent_->ui()->s3d_spinBox_active_slice->setMaximum(n_slices - 1);
    parent_->ui()->s3d_spinBox_active_slice->setValue(0);
    parent_->ui()->s3d_lineEdit_active_slice_depth->setText(QString::number(cube_.pos(depth_axis, 0), 'f', 2));

    auto ranges = GetRanges();
    auto step_sizes = GetStepSizes();
    auto n_steps = GetNSteps(ranges, step_sizes);

    switch (proj) {
    case PROJECTION::XY:
        projection_graph_->SetDataRange(ranges[Axis::X], ranges[Axis::Y], n_steps[Axis::X], n_steps[Axis::Y]);
        projection_graph_->SetPlotRange(ranges[Axis::X], ranges[Axis::Y], step_sizes[Axis::X], step_sizes[Axis::Y]);
        break;
    case PROJECTION::YZ:
        projection_graph_->SetDataRange(ranges[Axis::Y], ranges[Axis::Z], n_steps[Axis::Y], n_steps[Axis::Z]);
        projection_graph_->SetPlotRange(ranges[Axis::Y], ranges[Axis::Z], step_sizes[Axis::Y], step_sizes[Axis::Z]);
        break;
    case PROJECTION::ZX:
        projection_graph_->SetDataRange(ranges[Axis::Z], ranges[Axis::X], n_steps[Axis::Z], n_steps[Axis::X]);
        projection_graph_->SetPlotRange(ranges[Axis::Z], ranges[Axis::X], step_sizes[Axis::Z], step_sizes[Axis::X]);
        break;
    default:
        break;
    }
}

void Scanner3DGui::LoadProjection(PROJECTION proj) {
    InitProjection(proj);
    LoadSlice(proj, 0);
    SetAxisTitles(proj);
}

void Scanner3DGui::LoadSlice(PROJECTION proj, int slice) {
    qDebug() << "Scanner3DGui::LoadSlice PROJ " << static_cast<int>(proj) << " slice " << slice;
    Axis depth_axis = DepthAxis(proj);
    const int n_slices = cube_.nbins(depth_axis);
    if (slice >= n_slices || slice < 0) {
        qWarning() << "Scanner3DGui::LoadSlice slice index out of bounds";
        return;
    }
    parent_->ui()->s3d_spinBox_active_slice->setValue(slice);
    parent_->ui()->s3d_lineEdit_active_slice_depth->setText(QString::number(cube_.pos(depth_axis , slice), 'f', 2));
    QCPColorMapData map = cube_.GetColorMap(proj, slice);
    projection_graph_->SetData(map);
}

void Scanner3DGui::ClearAll() {
    ClearGraphs();
    ClearResults();
}

void Scanner3DGui::ClearResults() {

}

void Scanner3DGui::ClearGraphs() {
    try {
        chambre_histogram_->Clear("Reference");
        diode_histogram_->Clear("Diode");
        projection_graph_->Clear();
    }
    catch (std::exception& exc) {
        qWarning() << "Scanner3DGui::ClearGraphs Exception thrown: " << exc.what();
    }

}

void Scanner3DGui::ReadSignalNoise() {
    try {
        double noise = dev_factory_->GetSensor(SensorType::DIODE)->InterpretedValue(20);
        parent_->ui()->s3d_lineEdit_signal_noise->setText(QString::number(noise, 'f', 2));
        cube_.SetNoise(noise);
        DisplayCube();
    }
    catch (std::exception& exc) {
        qWarning() << "Scanner3DGui::ReadSignalNoise Exception thrown " << exc.what();
        QString msg = QString("Failed reaading noise: ") + exc.what();
        DisplayError(msg);
    }
}

void Scanner3DGui::SaveToDb() {
    try {
        SaveScanner3DDialog dialog(parent_);
        if (dialog.exec() != QDialog::Accepted) {
            qDebug() << "Scanner3DGui::SaveToDb User aborted save to db";
            return;
        }

        QDateTime timestamp(QDateTime::currentDateTime());
        cube_.SetTimestamp(timestamp);
        cube_.SetComment(dialog.comment());
        repo_->SaveCube(cube_);
        DisplayInfo("Results successfully saved");
    }
    catch (std::exception& exc) {
        qWarning() << "Scanner3DGui::SaveToDb Exception caught: " << exc.what();
        DisplayError(QString("Failed saving results to db: ") + exc.what());
    }
}

void Scanner3DGui::FetchFromDb() {
    try {
        FetchScanner3DDialog dialog(parent_, repo_);
        if (dialog.exec() != QDialog::Accepted) {
            qDebug() << "ScannerGui::FetchFromDb User aborted save to db";
            return;
        }
        cube_ = dialog.cube();
        auto configs = cube_.GetAxisConfigs();
        UpdateNoise(cube_.GetNoise());
        SetRanges(GetRanges(configs));
        SetStepSizes(GetStepSizes(configs));
        DisplayCube();
    }
    catch (std::exception& exc) {
        qWarning() << "ScannerGui::FetchFromDb Exception caught: " << exc.what();
        DisplayError(QString("Failed fetching cube from db: ") + exc.what());
    }
}

void Scanner3DGui::TurnOffConnectedButton() {
    parent_->ui()->s3d_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_red_button.png"));
}

void Scanner3DGui::TurnOnConnectedButton() {
    parent_->ui()->s3d_pushButton_connected->setStyleSheet(ButtonStyleSheet("connected_green_button.png"));
}

void Scanner3DGui::PrintMouseOver(int h_bin, int v_bin, double h_pos, double v_pos, double value) {
    parent_->ui()->s3d_lineEdit_bin->setText(QString::number(h_bin) + ", " + QString::number(v_bin));
    parent_->ui()->s3d_lineEdit_pos->setText(QString::number(h_pos, 'f', 2) + ", " + QString::number(v_pos, 'f', 2));
    parent_->ui()->s3d_lineEdit_signal->setText(QString::number(value, 'f', 2));
}

void Scanner3DGui::UpdateGraphs() {
}

PROJECTION Scanner3DGui::CurrentProjection() const {
    return static_cast<PROJECTION>(parent_->ui()->s3d_comboBox_projection->currentData().toInt());
}

Axis Scanner3DGui::DepthAxis(PROJECTION proj) const {
    Axis depth_axis = Axis::UNK;
    switch (proj) {
    case PROJECTION::XY:
        depth_axis = Axis::Z;
        break;
    case PROJECTION::YZ:
        depth_axis = Axis::X;
        break;
    case PROJECTION::ZX:
        depth_axis = Axis::Y;
        break;
    default:
        break;
    }
    return depth_axis;
}

void Scanner3DGui::SetAxisTitles(PROJECTION proj) {
    qDebug() << "Scanner3DGui::SetAxisTitles " << static_cast<int>(proj);
    switch (proj) {
    case PROJECTION::XY:
        projection_graph_->SetAxisTitles("X", "Y");
        break;
    case PROJECTION::YZ:
        projection_graph_->SetAxisTitles("Y", "Z");
        break;
    case PROJECTION::ZX:
        projection_graph_->SetAxisTitles("Z", "X");
        break;
    default:
        projection_graph_->SetAxisTitles("?", "?");
        break;
    }
}

std::map<Axis, Range> Scanner3DGui::GetRanges() const {
    std::map<Axis, Range> ranges;
    ranges[Axis::X] = Range(parent_->ui()->s3d_doubleSpinBox_x_lower->value(),
            parent_->ui()->s3d_doubleSpinBox_x_upper->value());
    ranges[Axis::Y] = Range(parent_->ui()->s3d_doubleSpinBox_y_lower->value(),
            parent_->ui()->s3d_doubleSpinBox_y_upper->value());
    ranges[Axis::Z] = Range(material::ToTissue(parent_->ui()->s3d_doubleSpinBox_z_lower->value(), IsMadeOf(QA_HARDWARE::SCANNER3D)),
                            material::ToTissue(parent_->ui()->s3d_doubleSpinBox_z_upper->value(), IsMadeOf(QA_HARDWARE::SCANNER3D)));
    return ranges;
}

void Scanner3DGui::SetRanges(const std::map<Axis, Range> ranges) {
    parent_->ui()->s3d_doubleSpinBox_x_lower->setValue(ranges.at(Axis::X).start());
    parent_->ui()->s3d_doubleSpinBox_x_upper->setValue(ranges.at(Axis::X).end());
    parent_->ui()->s3d_doubleSpinBox_y_lower->setValue(ranges.at(Axis::Y).start());
    parent_->ui()->s3d_doubleSpinBox_y_upper->setValue(ranges.at(Axis::Y).end());
    parent_->ui()->s3d_doubleSpinBox_z_lower->setValue(material::ToWater(ranges.at(Axis::Z).start(), IsMadeOf(QA_HARDWARE::SCANNER3D)));
    parent_->ui()->s3d_doubleSpinBox_z_upper->setValue(material::ToWater(ranges.at(Axis::Z).end(), IsMadeOf(QA_HARDWARE::SCANNER3D)));
}

std::map<Axis, double> Scanner3DGui::GetStepSizes() const {
    std::map<Axis, double> step_sizes;
    step_sizes[Axis::X] = parent_->ui()->s3d_doubleSpinBox_x_step->value();
    step_sizes[Axis::Y] = parent_->ui()->s3d_doubleSpinBox_y_step->value();
    step_sizes[Axis::Z] = material::ToTissue(parent_->ui()->s3d_doubleSpinBox_z_step->value(), IsMadeOf(QA_HARDWARE::SCANNER3D));
    return step_sizes;
}

double Scanner3DGui::GetNmbMeasurements() const {
    return parent_->ui()->s3d_spinBox_n_measurements->value();
}

double Scanner3DGui::GetMaxStdError() const {
    return parent_->ui()->s3d_spinBox_sem->value();
}

bool Scanner3DGui::GetFixedNmbMeasurements()  const {
    return parent_->ui()->s3d_radioButton_enable_fixed_nmb_measurements->isChecked();
}

void Scanner3DGui::SetStepSizes(const std::map<Axis, double> step_sizes) {
    parent_->ui()->s3d_doubleSpinBox_x_step->setValue(step_sizes.at(Axis::X));
    parent_->ui()->s3d_doubleSpinBox_y_step->setValue(step_sizes.at(Axis::Y));
    parent_->ui()->s3d_doubleSpinBox_z_step->setValue(material::ToWater(step_sizes.at(Axis::Z), IsMadeOf(QA_HARDWARE::SCANNER3D)));
}

std::map<Axis, int> Scanner3DGui::GetNSteps(const std::map<Axis, Range>& ranges,
                                            const std::map<Axis, double>& step_sizes) const {
    std::map<Axis, int> n_steps;
    n_steps[Axis::X] = 1 + static_cast<int>(std::round(ranges.at(Axis::X).length() / step_sizes.at(Axis::X)));
    n_steps[Axis::Y] = 1 + static_cast<int>(std::round(ranges.at(Axis::Y).length() / step_sizes.at(Axis::Y)));
    n_steps[Axis::Z] = 1 + static_cast<int>(std::round(ranges.at(Axis::Z).length() / step_sizes.at(Axis::Z)));
    return n_steps;
}

std::map<Axis, AxisConfig> Scanner3DGui::GetAxisConfigs(const std::map<Axis, int> n_steps,
                                                        const std::map<Axis, Range> ranges,
                                                        const std::map<Axis, double> step_sizes) const {
    std::map<Axis, AxisConfig> configs;
    configs[Axis::X] = AxisConfig(n_steps.at(Axis::X), ranges.at(Axis::X).start(), step_sizes.at(Axis::X));
    configs[Axis::Y] = AxisConfig(n_steps.at(Axis::Y), ranges.at(Axis::Y).start(), step_sizes.at(Axis::Y));
    configs[Axis::Z] = AxisConfig(n_steps.at(Axis::Z), ranges.at(Axis::Z).start(), step_sizes.at(Axis::Z));
    return configs;
}

std::map<Axis, Range> Scanner3DGui::GetRanges(const std::map<Axis, AxisConfig>& configs) {
    std::map<Axis, Range> ranges;
    ranges[Axis::X] = configs.at(Axis::X).range();
    ranges[Axis::Y] = configs.at(Axis::Y).range();
    ranges[Axis::Z] = configs.at(Axis::Z).range();
    return ranges;
}

std::map<Axis, double> Scanner3DGui::GetStepSizes(const std::map<Axis, AxisConfig>& configs) {
    std::map<Axis, double> step_sizes;
    step_sizes[Axis::X] = configs.at(Axis::X).step();
    step_sizes[Axis::Y] = configs.at(Axis::Y).step();
    step_sizes[Axis::Z] = configs.at(Axis::Z).step();
    return step_sizes;
}

std::map<Axis, int> Scanner3DGui::GetNbins(const std::map<Axis, AxisConfig>& configs) {
    std::map<Axis, int> n_bins;
    n_bins[Axis::X] = configs.at(Axis::X).nbins();
    n_bins[Axis::Y] = configs.at(Axis::Y).nbins();
    n_bins[Axis::Z] = configs.at(Axis::Z).nbins();
    return n_bins;
}

void Scanner3DGui::UpdateNoise(double noise) {
    parent_->ui()->s3d_lineEdit_signal_noise->setText(QString::number(noise, 'f', 2));
}
