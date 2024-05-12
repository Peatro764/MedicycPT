#include "ModulateurPage.h"

#include <QDebug>
#include <QMessageBox>

#include "ui_MTP.h"
#include "ColorHandler.h"
#include "MTP.h"
#include "Calc.h"
#include "MillingMachine.h"
#include "MCNPXData.h"
#include "MillingMachineProgramDialog.h"
#include "Material.h"

ModulateurPage::ModulateurPage(MTP* parent)
    : parent_(parent),
      modulateur_graph_(parent->ui()->modpage_modulateurCustomPlot)
{
    SetupLineEditValidators();
    ConnectSignals();
    InitPlot();
    SetDefaults();
}

ModulateurPage::~ModulateurPage()
{
}

void ModulateurPage::SetDefaults() {
    parent_->ui()->modPage_epsConvergenceLineEdit->setText("0.001");
    parent_->ui()->modPage_decalageLineEdit->setText("0.0");
    parent_->ui()->modPage_modulationStepLineEdit->setText("0.8");
    parent_->ui()->modPage_nSectorsLineEdit->setText("4");

    parent_->ui()->modPage_weightsTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    parent_->ui()->modPage_dataSetComboBox->addItem(QString("MCNPX"), static_cast<int>(DATASET::MCNPX));
    parent_->ui()->modPage_dataSetComboBox->addItem(QString("Shifted bragg"), static_cast<int>(DATASET::SHIFTEDBRAGG));
//    parent_->ui()->modPage_dataSetComboBox->setCurrentIndex(static_cast<int>(DATASET::SHIFTEDBRAGG));

    parent_->ui()->modPage_optimisationAlgorithmComboBox->addItem(QString("Joel99"), static_cast<int>(OPTALG::JOEL99));
}

void ModulateurPage::SetupLineEditValidators() {
    parent_->ui()->modPage_idLineEdit->setValidator(
                new QRegExpValidator(QRegExp("\\d{1,}"), parent_->ui()->modPage_idLineEdit));
    parent_->ui()->modPage_basePlateLineEdit->setValidator(
                new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->modPage_basePlateLineEdit));
    parent_->ui()->modPage_parcoursLineEdit->setValidator(
                new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->modPage_parcoursLineEdit));
    parent_->ui()->modPage_modulationLineEdit->setValidator(
                new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->modPage_modulationLineEdit));
    parent_->ui()->modPage_decalageLineEdit->setValidator(
                new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->modPage_decalageLineEdit));
    parent_->ui()->modPage_modulationStepLineEdit->setValidator(
                new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->modPage_modulationStepLineEdit));
    parent_->ui()->modPage_nSectorsLineEdit->setValidator(
                new QRegExpValidator(QRegExp("\\d{1,6}"), parent_->ui()->modPage_nSectorsLineEdit));
    parent_->ui()->modPage_epsConvergenceLineEdit->setValidator(
                new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->modPage_epsConvergenceLineEdit));
}

void ModulateurPage::ConnectSignals() {
    QObject::connect(parent_->ui()->modPage_createModulateurPushButton, SIGNAL(clicked()), this, SLOT(CreateModulateur()));
    QObject::connect(parent_->ui()->modPage_saveModulatorPushButton, SIGNAL(clicked()), this, SLOT(SaveModulatorToDb()));
    QObject::connect(parent_->ui()->modPage_saveMillingProgramPushButton, SIGNAL(clicked()), this, SLOT(SaveMillingProgramToDb()));
    QObject::connect(parent_->ui()->modPage_fetchFromDbPushButton, SIGNAL(clicked()), this, SLOT(FetchFromDatabase()));
    QObject::connect(parent_->ui()->modPage_degradeurLineEdit, SIGNAL(returnPressed()), this, SLOT(DegradeurUpdated()));
    QObject::connect(parent_->ui()->modPage_showFraiseuseProgramPushButton, SIGNAL(clicked()), this, SLOT(ShowFraiseuseProgram()));
}

void ModulateurPage::InitPlot() {
    parent_->ui()->modpage_sobpCustomPlot->legend->setVisible(false);
    parent_->ui()->modpage_sobpCustomPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    parent_->ui()->modpage_sobpCustomPlot->yAxis->setLabel("a.u");
    parent_->ui()->modpage_sobpCustomPlot->xAxis->setLabel("Depth [mm plexiglas]");
    //parent_->ui()->modpage_sobpCustomPlot->axisRect()->setupFullAxesBox();
    parent_->ui()->modpage_sobpCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);

    QColor fg_color = Qt::white;
    QColor bg_color = QColor("#31363b");

    parent_->ui()->modpage_sobpCustomPlot->setBackground(bg_color);

    parent_->ui()->modpage_sobpCustomPlot->yAxis->setLabelColor(fg_color);
    parent_->ui()->modpage_sobpCustomPlot->yAxis->setBasePen(QPen(fg_color, 1));
    parent_->ui()->modpage_sobpCustomPlot->yAxis->setTickPen(QPen(fg_color, 1));
    parent_->ui()->modpage_sobpCustomPlot->yAxis->setSubTickPen(QPen(fg_color, 1));
    parent_->ui()->modpage_sobpCustomPlot->yAxis->setTickLabelColor(fg_color);
    parent_->ui()->modpage_sobpCustomPlot->yAxis->grid()->setPen(QPen(fg_color, 0.5, Qt::DotLine));
    parent_->ui()->modpage_sobpCustomPlot->yAxis->grid()->setSubGridVisible(false);

    parent_->ui()->modpage_sobpCustomPlot->xAxis->setLabelColor(fg_color);
    parent_->ui()->modpage_sobpCustomPlot->xAxis->setBasePen(QPen(fg_color, 1));
    parent_->ui()->modpage_sobpCustomPlot->xAxis->setTickPen(QPen(fg_color, 1));
    parent_->ui()->modpage_sobpCustomPlot->xAxis->setSubTickPen(QPen(fg_color, 1));
    parent_->ui()->modpage_sobpCustomPlot->xAxis->setTickLabelColor(fg_color);
    parent_->ui()->modpage_sobpCustomPlot->xAxis->grid()->setPen(QPen(fg_color, 0.5, Qt::DotLine));
    parent_->ui()->modpage_sobpCustomPlot->xAxis->grid()->setSubGridVisible(false);
}

void ModulateurPage::CreateModulateur() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        InitModulateurMaker();
        um_plexi_weight weights(modulateur_maker_.GetInitialWeights());
        double convergence_criteria(parent_->ui()->modPage_epsConvergenceLineEdit->text().toDouble());
        const int totalrangeshift_umplexi = weights.begin()->first;
        const int baseplate_umplexi(calc::MM2UM(parent_->ui()->modPage_basePlateLineEdit->text().toDouble()));
        const int rangeshifter_umplexi(totalrangeshift_umplexi - baseplate_umplexi);
        if (rangeshifter_umplexi < 0.0) {
            throw std::runtime_error("Base plate thickness larger than total desired rangeshift");
        }

        parent_->ui()->modPage_degradeurLineEdit->setText(QString::number(calc::UM2MM(rangeshifter_umplexi), 'f', 1));

        for (int iter = 0; iter < 50; ++iter) {
            auto new_weights(modulateur_maker_.IterateWeights(weights));

            if (modulateur_maker_.HasConverged(new_weights, weights, convergence_criteria)) {
                UpdateTable(baseplate_umplexi,
                            SubtractFromKeys(new_weights, totalrangeshift_umplexi),
                            SubtractFromKeys(weights, totalrangeshift_umplexi));
                PlotCurves();
                DrawModulateur();
                return;
            } else {
                weights = new_weights;
            }
        }
        throw std::runtime_error("Max number of iterations exceeded");
    }
    catch (std::exception& exc) {
        qWarning() << "ModulateurPage::CreateModulateur Exception thrown: " << exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

um_plexi_weight ModulateurPage::SubtractFromKeys(const um_plexi_weight& weights, int value) const {
    um_plexi_weight new_weights;
    for (auto &w : weights) {
        new_weights.emplace(w.first - value, w.second);
    }
    return new_weights;
}

void ModulateurPage::InitModulateurMaker() {
    ClearData();
    const double parcours_mmtissue(parent_->ui()->modPage_parcoursLineEdit->text().toDouble());
    const double modulation_mmtissue(parent_->ui()->modPage_modulationLineEdit->text().toDouble());
    const double decalage_mmplexi(parent_->ui()->modPage_decalageLineEdit->text().toDouble());
    const double modulation_step(parent_->ui()->modPage_modulationStepLineEdit->text().toDouble());
    const int baseplate_umplexi(calc::MM2UM(parent_->ui()->modPage_basePlateLineEdit->text().toDouble()));

    DepthDoseCurve naked_bragg(parent_->repo()->GetTheoreticalBraggData().depth_dose_curves().at(0));
    const double parcours_max(material::Plexiglas2Tissue(naked_bragg.MaxParcours()));
    parent_->ui()->modPage_maxParcoursLineEdit->setText(QString::number(parcours_max, 'f', 2));

    std::vector<int> um_plexi_steps(ModulateurMaker::GetRequiredUmPlexiSteps(parcours_mmtissue, modulation_mmtissue, parcours_max,
                                                                             decalage_mmplexi, modulation_step));
    if (um_plexi_steps.at(0) < 0) {
        throw std::runtime_error("Decalage parameter exceeds maximum value for the given parcours");
    }

    if (um_plexi_steps.at(0) < baseplate_umplexi) {
        throw std::runtime_error("Baseplate thickness too large for the given parcours");
    }

    um_plexi_depthDoseCurve curves;
    for (int um_plexi : um_plexi_steps) {
        if (um_plexi == 0) {
            curves.emplace(um_plexi, naked_bragg);
        } else {
            // unit in bragg data is mm plexi
            curves.emplace(um_plexi, naked_bragg.Degrade(calc::UM2MM(um_plexi)));
        }
    }
    modulateur_maker_ = ModulateurMaker(curves);
}

//void ModulateurPage::InitModulateurMaker(const std::vector<int> um_plexi_steps) {
//    ClearData();
//    const int data_set(parent_->ui()->modPage_dataSetComboBox->currentIndex());

//    if (data_set == static_cast<int>(DATASET::MCNPX)) {
//        parent_->ui()->modpage_sobpCustomPlot->xAxis->setLabel("Depth [cm tissue]");
//        um_plexi_depthDoseCurve raw_curves(parent_->repo()->GetMCNPXData().UmPlexiDepthDoseCurves());
//        if (raw_curves.empty()) { throw std::runtime_error("No MCNPX depth dose curves retrieved"); }
//        const double parcours_max_mmtissue(raw_curves.at(0).MaxParcours() * 10.0);
//        parent_->ui()->modPage_maxParcoursLineEdit->setText(QString::number(parcours_max_mmtissue, 'f', 2));

//        um_plexi_depthDoseCurve interpolated_curves(ModulateurMaker::InterpolateDepthDoseCurves(raw_curves, um_plexi_steps));
//        modulateur_maker_ = ModulateurMaker(interpolated_curves);

//    } else if (data_set == static_cast<int>(DATASET::SHIFTEDBRAGG)) {
//        parent_->ui()->modpage_sobpCustomPlot->xAxis->setLabel("Depth [mm plexiglas]");
//        DepthDoseCurve naked_bragg(parent_->repo()->GetTheoreticalBraggData().depth_dose_curves().at(0));
//        const double parcours_max(calc::Plexiglas2Tissue(naked_bragg.MaxParcours()));
//        parent_->ui()->modPage_maxParcoursLineEdit->setText(QString::number(parcours_max, 'f', 2));

//        um_plexi_depthDoseCurve curves;
//        for (int um_plexi : um_plexi_steps) {
//            if (um_plexi == 0) {
//                curves.emplace(um_plexi, naked_bragg);
//            } else {
//                // unit in bragg data is mm plexi
//                curves.emplace(um_plexi, naked_bragg.Degrade(calc::UM2MM(um_plexi)));
//            }
//        }
//        modulateur_maker_ = ModulateurMaker(curves);
//    } else {
//        throw std::runtime_error("Selected dataset not implemented");
//    }
//}

void ModulateurPage::UpdateTable(int baseplate_um, const um_plexi_weight& new_weights, const um_plexi_weight& old_weights) {
    parent_->ui()->modPage_weightsTableWidget->clearContents();
    parent_->ui()->modPage_weightsTableWidget->setRowCount((int)new_weights.size());
    int row(0);
    for (auto& w : new_weights) {
        parent_->ui()->modPage_weightsTableWidget->setRowHeight(row, 40);
        parent_->ui()->modPage_weightsTableWidget->setItem(row, COL::MMPLEXI, new QTableWidgetItem(QString::number(calc::UM2MM(w.first + baseplate_um))));
        QDoubleSpinBox *spinBox = new QDoubleSpinBox(parent_);
        spinBox->setDecimals(3);
        spinBox->setMinimum(0.0);
        spinBox->setMaximum(10.0);
        spinBox->setSingleStep(0.01);
        spinBox->setValue(w.second);
        QObject::connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(UpdateAngleColumn()));
        QObject::connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(DrawModulateur()));
        QObject::connect(spinBox, SIGNAL(valueChanged(double)), this, SLOT(PlotCurves()));
        parent_->ui()->modPage_weightsTableWidget->setCellWidget(row, COL::WEIGHT, spinBox );
        parent_->ui()->modPage_weightsTableWidget->setItem(row, COL::DWEIGHT, new QTableWidgetItem(QString::number(w.second - old_weights.at(w.first), 'f', 4)));
        row++;
    }
    UpdateAngleColumn();
}

void ModulateurPage::UpdateAngleColumn() {
    um_plexi_weight weights(GetWeightsFromTable());
    const int n_sectors(parent_->ui()->modPage_nSectorsLineEdit->text().toInt());
    auto angular_sizes(Modulateur::GetSectorAngularSizes(weights, n_sectors));
    int row(0);
    for (auto& w : weights) {
        parent_->ui()->modPage_weightsTableWidget->setItem(row, COL::ANGLE, new QTableWidgetItem(QString::number(angular_sizes.at(w.first), 'f', 1)));
        row++;
    }
}

um_plexi_weight ModulateurPage::GetWeightsFromTable() const {
    um_plexi_weight weights;
    for (int row = 0; row < parent_->ui()->modPage_weightsTableWidget->rowCount(); ++row) {
        int um_plexi(calc::MM2UM(parent_->ui()->modPage_weightsTableWidget->item(row, 0)->text().toDouble()));
        double weight(static_cast<QDoubleSpinBox*>(parent_->ui()->modPage_weightsTableWidget->cellWidget(row, COL::WEIGHT))->value());
        weights.emplace(um_plexi, weight);
    }
    return weights;
}

um_plexi_weight ModulateurPage::GetAnglesFromTable() const {
    um_plexi_weight angles;
    for (int row = 0; row < parent_->ui()->modPage_weightsTableWidget->rowCount(); ++row) {
        int um_plexi(calc::MM2UM(parent_->ui()->modPage_weightsTableWidget->item(row, COL::MMPLEXI)->text().toDouble()));
        double angle(parent_->ui()->modPage_weightsTableWidget->item(row, COL::ANGLE)->text().toDouble());
        angles.emplace(um_plexi, angle);
    }
    return angles;
}

um_plexi_depthDoseCurve ModulateurPage::GetDepthDoseCurves(const std::vector<int>& um_plexi) {
    um_plexi_depthDoseCurve curves;
    DepthDoseCurve naked_bragg(parent_->repo()->GetTheoreticalBraggData().depth_dose_curves().at(0));
    for (int um : um_plexi) {
        if (um == 0) {
            curves.emplace(um, naked_bragg);
        } else {
            // unit in bragg data is mm plexi
            curves.emplace(um, naked_bragg.Degrade(calc::UM2MM(um)));
        }
    }
    return curves;
}

void ModulateurPage::PlotCurves() {
    try {
        parent_->ui()->modpage_sobpCustomPlot->clearPlottables();

        um_plexi_weight umPlexiWeight(GetWeightsFromTable());
        int um_rangeshifter = calc::MM2UM(parent_->ui()->modPage_degradeurLineEdit->text().toDouble());
        std::vector<int> umPlexiWithRangeShifter;
        for (auto& w : umPlexiWeight) {
                umPlexiWithRangeShifter.push_back(w.first + um_rangeshifter);
        }
        um_plexi_depthDoseCurve umPlexiDepthDoseCurves = GetDepthDoseCurves(umPlexiWithRangeShifter);        

        std::vector<double> monitor_units;
        std::vector<DepthDoseCurve> depth_dose_curves;
        std::vector<int> um_plexi;
        std::vector<double> weights;
        for (auto& w : umPlexiWeight) {
            monitor_units.push_back(1.0);
            int um = w.first + um_rangeshifter;
            depth_dose_curves.push_back(umPlexiDepthDoseCurves.at(um));
            um_plexi.push_back(um);
            weights.push_back(w.second);
        }

        SOBPCurve sobp(depth_dose_curves, monitor_units, um_plexi, weights, false);

        std::vector<DepthDoseCurve> ind_weighted_curves = sobp.IndividualWeightedCurves();
        std::vector<QColor> colors(colorhandler::GenerateColors((int)ind_weighted_curves.size()));
        int color_index(0);
        for (auto& c : ind_weighted_curves) {
            PlotDepthDoseCurve(c, colors.at(color_index++));
        }
        auto sum_curve = sobp.Curve();
        PlotSumCurve(sum_curve);
        parent_->ui()->modPage_penumbraLineEdit->setText(QString::number(sum_curve.Penumbra(), 'f', 2));
        parent_->ui()->modPage_flatnessLineEdit->setText(QString::number(sum_curve.Flatness(), 'f', 2));

        parent_->ui()->modpage_sobpCustomPlot->rescaleAxes(true);
        parent_->ui()->modpage_sobpCustomPlot->yAxis->setRangeUpper(1.2 * parent_->ui()->modpage_sobpCustomPlot->yAxis->range().upper);
        parent_->ui()->modpage_sobpCustomPlot->legend->setFont(QFont("Helvetica", 9));
        parent_->ui()->modpage_sobpCustomPlot->legend->setRowSpacing(-3);
        //        parent_->ui()->modpage_sobpCustomPlot->legend->setVisible(true);
//        if (sum_curve.MaxParcours() > 20) {
//            parent_->ui()->modpage_sobpCustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignBottom);
//        } else {
//            parent_->ui()->modpage_sobpCustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop);
//        }

        PlotDesiredParcours();
        parent_->ui()->modpage_sobpCustomPlot->replot();
    }
    catch (std::exception& exc) {
        qWarning() << "ModulateurPage::PlotSOBPCurve Exception thrown: " << exc.what();
        QMessageBox::warning(parent_, QString("MTP"), QString("Failed plotting sobp curve: ") + exc.what());
    }
}

void ModulateurPage::PlotDesiredParcours() {
    auto graph = parent_->ui()->modpage_sobpCustomPlot->addGraph();
    QPen pen;
    pen.setColor(Qt::red);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    graph->setPen(pen);
    const double desired_parcours_mmtissue(parent_->ui()->modPage_parcoursLineEdit->text().toDouble());
    graph->addData(material::Tissue2Plexiglas(desired_parcours_mmtissue), parent_->ui()->modpage_sobpCustomPlot->yAxis->range().lower);
    graph->addData(material::Tissue2Plexiglas(desired_parcours_mmtissue), parent_->ui()->modpage_sobpCustomPlot->yAxis->range().upper);
    graph->setName("Desired parcours");
    graph->setVisible(true);
}

void ModulateurPage::PlotDepthDoseCurve(/*int um_plexi,*/ const DepthDoseCurve& curve, const QColor& color) {
    auto graph = parent_->ui()->modpage_sobpCustomPlot->addGraph();
    QPen pen;
    pen.setColor(color);
    pen.setWidth(1);
    graph->setPen(pen);
  //  graph->setBrush(QBrush(color));
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, color, color, 3));
    graph->removeFromLegend(parent_->ui()->modpage_sobpCustomPlot->legend);

    QVector<double> keys;
    QVector<double> values;
    for (DepthDose& depth_dose : curve.DepthDoses()) {
        keys.push_back(depth_dose.depth());
        values.push_back(depth_dose.dose());
    }
    graph->setData(keys, values, false);
    graph->setVisible(true);
//    graph->setName(QString::number(calc::UM2MM(um_plexi), 'f', 1));
}

void ModulateurPage::PlotSumCurve(const DepthDoseCurve& curve) {
    auto graph = parent_->ui()->modpage_sobpCustomPlot->addGraph();
    QPen pen;
    QColor color = QColor("#3daee9");
    pen.setColor(color);
    pen.setWidth(2);
    graph->setPen(pen);
//    graph->setBrush(QBrush(Qt::white));
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, color, color, 4));

    QVector<double> keys;
    QVector<double> values;
    for (DepthDose& depth_dose : curve.DepthDoses()) {
        keys.push_back(depth_dose.depth());
        values.push_back(depth_dose.dose());
    }
    graph->setData(keys, values, false);
    graph->setVisible(true);
    graph->setName("Weighted sum");
}

void ModulateurPage::DrawModulateur() {
    modulateur_graph_.Plot(GetAnglesFromTable());
}

void ModulateurPage::SaveModulatorToDb() {
    QMessageBox::warning(parent_, "MTP", "Currently disabled");
    return;
//    try {
//        bool ok(false);
//        int id = QInputDialog::getInt(parent_, "MTP", "Modulateur ID:", 0, 0, 100000, 1, &ok);
//        if (!ok) { return; }

//        Modulateur modulateur(ConstructModulateurFromData(id));
//        bool update_existing(false);
//        if (parent_->repo()->ModulateurExist(id)) {
//            QMessageBox msgBox;
//            msgBox.setText("Modulateur exists\n\nDo you want to overwrite the current one?");
//            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//            msgBox.setDefaultButton(QMessageBox::No);
//            switch (msgBox.exec()) {
//            case QMessageBox::Yes:
//                update_existing = true;
//                break;
//            case QMessageBox::No:
//                return;
//                break;
//            default:
//                return;
//                break;
//            }
//        }

//        Fraiseuse fraiseuse;
//        fraiseuse.CreateProgram(modulateur);
//        parent_->repo()->SaveModulateurItems(modulateur, fraiseuse.GetProgram(), update_existing);
//        QMessageBox::information(parent_, "MTP", QString("The modulateur was successfully saved to database.\n\n"
//                                                         "Mod100, mod98 and parcours have been set to zero.\n"
//                                                         "Available flag has been set to false."));
//    }
//    catch (std::exception& exc) {
//        QMessageBox::warning(parent_, "MTP", "Could not save modulateur: " + QString(exc.what()));
//    }
}

void ModulateurPage::SaveMillingProgramToDb() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        Modulateur modulateur = ConstructModulateurFromData();
        MillingMachine milling_machine;
        if (!milling_machine.CreateProgram(modulateur)) throw std::runtime_error("Echec de la création du programme");

        parent_->repo()->SaveTreatmentFraiseuseProgram(modulateur.id().toInt(), Modulateur::program_type, QDateTime::currentDateTime(), milling_machine.GetProgram());
        QMessageBox::information(parent_, "MTP", QString("Le programme de fraisage a été enregistré dans la base de données"));
    }
    catch (std::exception& exc) {
        qWarning() << QString("ModulateurPage::SaveMillingProgramToDb Exception thrown: ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception levée: " + QString(exc.what()));
    }
}

void ModulateurPage::FetchFromDatabase() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        ClearData();
        bool ok(false);
        int id = QInputDialog::getInt(parent_, "MTP", "Modulateur ID:", 0, 0, 100000, 1, &ok);
        if (!ok) { return; }

        if (!parent_->repo()->ModulateurExist(id)) {
            QMessageBox::information(parent_, "MTP", QString("Le modulateur n'existe pas"));
            return;
        }

        Modulateur mod(parent_->repo()->GetModulateur(id));
        parent_->ui()->modPage_idLineEdit->setText(mod.id());
        parent_->ui()->modPage_basePlateLineEdit->setText(QString::number(calc::UM2MM(mod.RangeShift()), 'f', 2));
        parent_->ui()->modPage_parcoursLineEdit->setText(QString::number(mod.parcours(), 'f', 1));
        parent_->ui()->modPage_modulationLineEdit->setText(QString::number(mod.mod100(), 'f', 1));
        parent_->ui()->modPage_decalageLineEdit->setText(QString::number(mod.decalage(), 'f', 1));
        parent_->ui()->modPage_epsConvergenceLineEdit->setText(QString::number(mod.conv_criteria(), 'f', 5));
        parent_->ui()->modPage_modulationStepLineEdit->setText(QString::number(mod.mod_step(), 'f', 1));
        parent_->ui()->modPage_dataSetComboBox->setCurrentText(mod.data_set());
        parent_->ui()->modPage_optimisationAlgorithmComboBox->setCurrentText(mod.opt_alg());
        parent_->ui()->modPage_nSectorsLineEdit->setText(QString::number(mod.n_sectors()));
        const double max_range_mmtissue = GetMaxRangeMMTissue(mod.data_set());
        parent_->ui()->modPage_maxParcoursLineEdit->setText(QString::number(max_range_mmtissue, 'f', 2));
        const double range_shifter_mm_plexi = std::max(0.0, material::Tissue2Plexiglas(max_range_mmtissue - mod.parcours())
                - calc::UM2MM(mod.RangeShift()));
        parent_->ui()->modPage_degradeurLineEdit->setText(QString::number(range_shifter_mm_plexi, 'f', 1));
        UpdateTable(mod.RangeShift(), mod.um_plexi_weight(), mod.um_plexi_weight());
        DrawModulateur();
        PlotCurves();
    }
    catch (std::exception& exc) {
        qWarning() << QString("ModulateurPage::FetchProgramFromDatabase Exception thrown: ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

double ModulateurPage::GetMaxRangeMMTissue(QString data_set) const {
    try {
        if (data_set == QString("MCNPX")) {
            throw std::runtime_error("GetMaxRange not implemented for mcnpx data set");
        } else if (data_set == QString("Shifted bragg")) {
            DepthDoseCurve naked_bragg(parent_->repo()->GetTheoreticalBraggData().depth_dose_curves().at(0));
            return material::Plexiglas2Tissue(naked_bragg.MaxParcours());
        } else {
            throw std::runtime_error("DATASET unknown");
        }
    }
    catch (std::exception& exc) {
        throw;
    }
}

Modulateur ModulateurPage::ConstructModulateurFromData() {
    if (parent_->ui()->modPage_parcoursLineEdit->text().isEmpty()) throw std::runtime_error("No parcours");
    const double input_parcours(parent_->ui()->modPage_parcoursLineEdit->text().toDouble());
    if (parent_->ui()->modPage_modulationLineEdit->text().isEmpty()) throw std::runtime_error("No modulation");
    const double input_modulation(parent_->ui()->modPage_modulationLineEdit->text().toDouble());
    if (parent_->ui()->modPage_decalageLineEdit->text().isEmpty()) throw std::runtime_error("No decalage");
    const double decalage(parent_->ui()->modPage_decalageLineEdit->text().toDouble());
    if (parent_->ui()->modPage_epsConvergenceLineEdit->text().isEmpty()) throw std::runtime_error("No convergence criteria");
    const double conv_criteria(parent_->ui()->modPage_epsConvergenceLineEdit->text().toDouble());
    if (parent_->ui()->modPage_modulationStepLineEdit->text().isEmpty()) throw std::runtime_error("No modulation step");
    const double modulation_step(parent_->ui()->modPage_modulationStepLineEdit->text().toDouble());
    const QString data_set(parent_->ui()->modPage_dataSetComboBox->currentText());
    const QString opt_alg(parent_->ui()->modPage_optimisationAlgorithmComboBox->currentText());
    if (parent_->ui()->modPage_idLineEdit->text().isEmpty()) throw std::runtime_error("No modulator id");
    const QString id(parent_->ui()->modPage_idLineEdit->text());
    if (parent_->ui()->modPage_basePlateLineEdit->text().isEmpty()) throw std::runtime_error("No baseplate info");
    const int um_degradeur(calc::MM2UM(parent_->ui()->modPage_basePlateLineEdit->text().toDouble()));
    if (parent_->ui()->modPage_nSectorsLineEdit->text().isEmpty()) throw std::runtime_error("No n_sectors");
    const int n_sectors(parent_->ui()->modPage_nSectorsLineEdit->text().toInt());
    um_plexi_weight weights(GetWeightsFromTable());
    if (weights.empty()) throw std::runtime_error("No modulateur weights");

    return Modulateur(id, input_modulation, input_modulation, input_parcours, um_degradeur, weights, false,
                      opt_alg, data_set, n_sectors, modulation_step, input_parcours, input_modulation,
                      decalage, conv_criteria);
}

void ModulateurPage::ClearData() {
    parent_->ui()->modPage_weightsTableWidget->clearContents();
    parent_->ui()->modPage_degradeurLineEdit->clear();
    parent_->ui()->modPage_penumbraLineEdit->clear();
    parent_->ui()->modPage_flatnessLineEdit->clear();
    modulateur_graph_.Clear();
    parent_->ui()->modpage_modulateurCustomPlot->clearGraphs();
}

void ModulateurPage::ShowFraiseuseProgram() {
    try {
       MillingMachine milling_machine;
       if (!milling_machine.CreateProgram(ConstructModulateurFromData())) {
           throw std::runtime_error("Non-valid parameters");
       }
       MillingMachineProgramDialog dialog(parent_, milling_machine.GetProgram());
       dialog.exec();
    }
    catch (std::exception& exc) {
         QMessageBox::warning(parent_, QString("MTP"), "Failed creating program: " + QString(exc.what()));
    }
}

void ModulateurPage::DegradeurUpdated() {
    DrawModulateur();
    PlotCurves();
}
