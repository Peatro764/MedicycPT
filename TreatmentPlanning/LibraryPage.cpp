#include "LibraryPage.h"

#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>

#include "ui_MTP.h"
#include "MTP.h"
#include "Calc.h"
#include "Util.h"
#include "AssociateDialog.h"
#include "Material.h"


LibraryPage::LibraryPage(MTP* parent)
    : parent_(parent),
      zhalf_tracer(nullptr),
      ref_zhalf_tracer(nullptr)
{
    SetupLineEditValidators();
    ConnectSignals();
    SetupDefaults();
    ConfigureTables();
    ConfigurePlot();
}

LibraryPage::~LibraryPage()
{
}

void LibraryPage::SetupLineEditValidators() {
    parent_->ui()->moddeglib_maxParcoursLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_maxParcoursLineEdit));
    parent_->ui()->moddeglib_desModLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_desModLineEdit));
    parent_->ui()->moddeglib_desParcoursLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_desParcoursLineEdit));
    parent_->ui()->moddeglib_minDMod98LineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_minDMod98LineEdit));
    parent_->ui()->moddeglib_maxDMod98LineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_maxDMod98LineEdit));
    parent_->ui()->moddeglib_minDMod100LineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_minDMod100LineEdit));
    parent_->ui()->moddeglib_maxDMod100LineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_maxDMod100LineEdit));
    parent_->ui()->moddeglib_minDParcoursLineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_minDParcoursLineEdit));
    parent_->ui()->moddeglib_maxDParcoursLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->moddeglib_maxDParcoursLineEdit));
}

void LibraryPage::ConnectSignals() {
    QObject::connect(parent_->ui()->moddeglib_queryModulateurPushButton, SIGNAL(clicked()), this, SLOT(ListModulators()));
//    QObject::connect(parent_->ui()->moddeglib_queryModulateurPushButton, SIGNAL(clicked()), this, SLOT(ListDegradeurs()));
    QObject::connect(parent_->ui()->moddeglib_calculateSOBPPushButton, SIGNAL(clicked()), this, SLOT(CalculateSOBPCurves()));
    QObject::connect(parent_->ui()->moddeglib_sobpCustomPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    QObject::connect(parent_->ui()->moddeglib_sobpCustomPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    QObject::connect(parent_->ui()->moddeglib_modulatorProposalTableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(ListDegradeurs(int)));
    QObject::connect(parent_->ui()->moddeglib_modulatorProposalTableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(SelectModulateur(int)));
    QObject::connect(parent_->ui()->moddeglib_degradeurProposalTableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(SelectDegradeur(int)));
    QObject::connect(parent_->ui()->moddeglib_degradeurProposalTableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(CalculateSOBPCurves()));
    QObject::connect(parent_->ui()->moddeglib_associateToDossierPushButton, SIGNAL(clicked()), this, SLOT(AssociateToDossier()));
}

void LibraryPage::SetupDefaults() {
    parent_->ui()->moddeglib_maxParcoursLineEdit->setText("30.7");
    parent_->ui()->moddeglib_desModLineEdit->setText("0.0");
    parent_->ui()->moddeglib_desParcoursLineEdit->setText("0.0");
    parent_->ui()->moddeglib_minDMod98LineEdit->setText("-0.1");
    parent_->ui()->moddeglib_maxDMod98LineEdit->setText("0.5");
    parent_->ui()->moddeglib_minDMod100LineEdit->setText("-0.5");
    parent_->ui()->moddeglib_maxDMod100LineEdit->setText("0.5");
    parent_->ui()->moddeglib_minDParcoursLineEdit->setText("-3.0");
    parent_->ui()->moddeglib_maxDParcoursLineEdit->setText("3.0");
}

void LibraryPage::ConfigureTables() {
    parent_->ui()->moddeglib_modulatorProposalTableWidget->horizontalHeader()->setFont(QFont("Sans Serif", 7));
    parent_->ui()->moddeglib_modulatorProposalTableWidget->horizontalHeader()->setFixedHeight(30);
    QStringList mod_headerLabels;
    mod_headerLabels << "ID" << QString::fromUtf8("Mod100/98\n[mm tissue]") <<
                    "Parcours\n[mm tissue]" << "Degr\n[mm plexi]";
    parent_->ui()->moddeglib_modulatorProposalTableWidget->setHorizontalHeaderLabels(mod_headerLabels);
    parent_->ui()->moddeglib_modulatorProposalTableWidget->horizontalHeader()->resizeSection(MOD_ID, 40);
    parent_->ui()->moddeglib_modulatorProposalTableWidget->horizontalHeader()->resizeSection(MOD_MOD, 75);
    parent_->ui()->moddeglib_modulatorProposalTableWidget->horizontalHeader()->resizeSection(MOD_PARC, 75);
    parent_->ui()->moddeglib_modulatorProposalTableWidget->horizontalHeader()->resizeSection(MOD_DEGR, 75);

    parent_->ui()->moddeglib_degradeurProposalTableWidget->horizontalHeader()->setFont(QFont("Sans Serif", 7));
    parent_->ui()->moddeglib_degradeurProposalTableWidget->horizontalHeader()->setFixedHeight(30);
    QStringList deg_headerLabels;
    deg_headerLabels << "ID(s)\n[mm plexi]" << QString::fromUtf8("Degr\n[mm tissue]") << "ΔDegr\n[mm tissue]";
    parent_->ui()->moddeglib_degradeurProposalTableWidget->setHorizontalHeaderLabels(deg_headerLabels);
    parent_->ui()->moddeglib_degradeurProposalTableWidget->horizontalHeader()->resizeSection(DEG_ID, 71);
    parent_->ui()->moddeglib_degradeurProposalTableWidget->horizontalHeader()->resizeSection(DEG_DEGR, 71);
    parent_->ui()->moddeglib_degradeurProposalTableWidget->horizontalHeader()->resizeSection(DEG_DELTADEGR, 71);
    parent_->ui()->moddeglib_degradeurProposalTableWidget->horizontalHeader()->show();
}

void LibraryPage::ConfigurePlot() {
    QColor axis_color = Qt::white;
    QColor background_color = QColor("#31363b");

    parent_->ui()->moddeglib_sobpCustomPlot->setBackground(background_color);
    parent_->ui()->moddeglib_sobpCustomPlot->legend->setVisible(false);
    parent_->ui()->moddeglib_sobpCustomPlot->legend->setBrush(background_color);
    parent_->ui()->moddeglib_sobpCustomPlot->legend->setTextColor(axis_color);
    parent_->ui()->moddeglib_sobpCustomPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    parent_->ui()->moddeglib_sobpCustomPlot->yAxis->setLabel("(D/UM)_SOBP [cGy/UM]");
    parent_->ui()->moddeglib_sobpCustomPlot->xAxis->setLabel("Depth [cm]");
    //parent_->ui()->moddeglib_sobpCustomPlot->axisRect()->setupFullAxesBox();
    parent_->ui()->moddeglib_sobpCustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    parent_->ui()->moddeglib_sobpCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);

    parent_->ui()->moddeglib_sobpCustomPlot->yAxis->setLabelColor(axis_color);
    parent_->ui()->moddeglib_sobpCustomPlot->yAxis->setBasePen(QPen(axis_color, 1));
    parent_->ui()->moddeglib_sobpCustomPlot->yAxis->setTickPen(QPen(axis_color, 1));
    parent_->ui()->moddeglib_sobpCustomPlot->yAxis->setSubTickPen(QPen(axis_color, 1));
    parent_->ui()->moddeglib_sobpCustomPlot->yAxis->setTickLabelColor(axis_color);
    parent_->ui()->moddeglib_sobpCustomPlot->yAxis->grid()->setPen(QPen(axis_color, 0.5, Qt::DotLine));
    parent_->ui()->moddeglib_sobpCustomPlot->yAxis->grid()->setSubGridVisible(false);

    parent_->ui()->moddeglib_sobpCustomPlot->xAxis->setLabelColor(axis_color);
    parent_->ui()->moddeglib_sobpCustomPlot->xAxis->setBasePen(QPen(axis_color, 1));
    parent_->ui()->moddeglib_sobpCustomPlot->xAxis->setTickPen(QPen(axis_color, 1));
    parent_->ui()->moddeglib_sobpCustomPlot->xAxis->setSubTickPen(QPen(axis_color, 1));
    parent_->ui()->moddeglib_sobpCustomPlot->xAxis->setTickLabelColor(axis_color);
    parent_->ui()->moddeglib_sobpCustomPlot->xAxis->grid()->setPen(QPen(axis_color, 0.5, Qt::DotLine));
    parent_->ui()->moddeglib_sobpCustomPlot->xAxis->grid()->setSubGridVisible(false);
}

void LibraryPage::ListModulators() {

    parent_->ui()->moddeglib_modulatorProposalTableWidget->clearContents();
    parent_->ui()->moddeglib_degradeurProposalTableWidget->clearContents();

    const double des_parcours = parent_->ui()->moddeglib_desParcoursLineEdit->text().toDouble();
    const double des_modulation = parent_->ui()->moddeglib_desModLineEdit->text().toDouble();

    const double min_mod100 = parent_->ui()->moddeglib_enableMod100Selection_RadioButton->isChecked() ? des_modulation + parent_->ui()->moddeglib_minDMod100LineEdit->text().toDouble() : -1000.0;
    const double max_mod100 = parent_->ui()->moddeglib_enableMod100Selection_RadioButton->isChecked() ? des_modulation + parent_->ui()->moddeglib_maxDMod100LineEdit->text().toDouble() : 1000.0;
    const double min_mod98 = parent_->ui()->moddeglib_enableMod98Selection_RadioButton->isChecked() ? des_modulation + parent_->ui()->moddeglib_minDMod98LineEdit->text().toDouble() : -1000.0;
    const double max_mod98 = parent_->ui()->moddeglib_enableMod98Selection_RadioButton->isChecked() ? des_modulation + parent_->ui()->moddeglib_maxDMod98LineEdit->text().toDouble() : 1000.0;
    const double min_parcours = parent_->ui()->moddeglib_enableParcoursSelection_RadioButton->isChecked() ? des_parcours + parent_->ui()->moddeglib_minDParcoursLineEdit->text().toDouble() : -1000.0;
    const double max_parcours = parent_->ui()->moddeglib_enableParcoursSelection_RadioButton->isChecked() ? des_parcours + parent_->ui()->moddeglib_maxDParcoursLineEdit->text().toDouble() : 1000.0;

    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        auto modulateurs = parent_->repo()->GetModulateurs(min_mod100, max_mod100,
                                                           min_mod98, max_mod98,
                                                           min_parcours, max_parcours);
        if (modulateurs.empty()) {
            parent_->ui()->moddeglib_modulatorProposalTableWidget->clearContents();
            QMessageBox::information(parent_, "Empty result", "No modulators found");
        } else {
            SortModulateurs(modulateurs, des_modulation, des_parcours);
            WriteModulateursInTable(modulateurs);
        }
    }
    catch (std::exception& exc) {
        qWarning() << QString("MTP::ListModulators An exception was thrown ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void LibraryPage::SortModulateurs(std::vector<Modulateur>& modulateurs, double des_modulation, double des_parcours) {
    auto sortRule = [&] (const Modulateur& m1, const Modulateur& m2) -> bool
    { return ((std::abs(std::abs(m1.mod100() - des_modulation) - std::abs(m2.mod100() - des_modulation)) < 0.01 && std::abs(m1.parcours() - des_parcours) < std::abs(m2.parcours() - des_parcours)) ||
                std::abs(m1.mod100() - des_modulation) < std::abs(m2.mod100() - des_modulation)); };

    std::sort(modulateurs.begin(), modulateurs.end(), sortRule);
}

void LibraryPage::WriteModulateursInTable(const std::vector<Modulateur>& modulateurs) {
    for (int idx = 0; idx < std::min((int)modulateurs.size(), parent_->ui()->moddeglib_modulatorProposalTableWidget->rowCount()); ++idx) {
        parent_->ui()->moddeglib_modulatorProposalTableWidget->setItem(idx, MOD_ID, new QTableWidgetItem(modulateurs.at(idx).id()));
        parent_->ui()->moddeglib_modulatorProposalTableWidget->setItem(idx, MOD_MOD, new QTableWidgetItem(QString::number(modulateurs.at(idx).mod100(), 'f', 1)  + "/" + QString::number(modulateurs.at(idx).mod98(), 'f', 1)));
        parent_->ui()->moddeglib_modulatorProposalTableWidget->setItem(idx, MOD_PARC, new QTableWidgetItem(QString::number(modulateurs.at(idx).parcours(), 'f', 2)));
        parent_->ui()->moddeglib_modulatorProposalTableWidget->setItem(idx, MOD_DEGR, new QTableWidgetItem(QString::number((double)modulateurs.at(idx).RangeShift() / 1000.0, 'f', 2)));
    }
}

void LibraryPage::ListDegradeurs() {
    if (parent_->ui()->moddeglib_desParcoursLineEdit->text().isEmpty() || parent_->ui()->moddeglib_desParcoursLineEdit->text().isNull()) {
        return;
    }
    ListDegradeurs(parent_->ui()->moddeglib_desParcoursLineEdit->text().toDouble());
}

void LibraryPage::ListDegradeurs(int modulateur_row) {
    if (!parent_->ui()->moddeglib_modulatorProposalTableWidget->item(modulateur_row, MOD_DEGR)) {
        QMessageBox::warning(parent_, "Unvalid choice", "Missing modulateur data on chosen row");
        return;
    }
    if (parent_->ui()->moddeglib_desParcoursLineEdit->text().isEmpty() || parent_->ui()->moddeglib_desParcoursLineEdit->text().isNull()) {
        return;
    }
    QString cell_moddeg(parent_->ui()->moddeglib_modulatorProposalTableWidget->item(modulateur_row, MOD_DEGR)->text());
    if (cell_moddeg.isNull() || cell_moddeg.isEmpty()) {
        QMessageBox::warning(parent_, "Unvalid choice", "Nonvalid modulateur degr on chosen row");
        return;
    }

    const double des_parcours_mmtissue(parent_->ui()->moddeglib_desParcoursLineEdit->text().toDouble());
    const double modulateur_degradeur_mmtissue(material::Plexiglas2Tissue(cell_moddeg.toDouble()));
    ListDegradeurs(des_parcours_mmtissue + modulateur_degradeur_mmtissue);
}

void LibraryPage::ListDegradeurs(double des_deg_mmtissue) {
    if (parent_->ui()->moddeglib_maxDParcoursLineEdit->text().isEmpty() || parent_->ui()->moddeglib_maxDParcoursLineEdit->text().isNull()) {
        QMessageBox::warning(parent_, "Missing input", "Max parcours difference not given");
        return;
    }
    if (parent_->ui()->moddeglib_maxParcoursLineEdit->text().isEmpty() || parent_->ui()->moddeglib_maxParcoursLineEdit->text().isNull()) {
        QMessageBox::warning(parent_, "Missing input", "Max parcours not given");
        return;
    }

    const double max_parcours_mmtissue(parent_->ui()->moddeglib_maxParcoursLineEdit->text().toDouble());
    const int des_um_plexi = (int)(1000.0 * material::Tissue2Plexiglas(max_parcours_mmtissue - des_deg_mmtissue));
    const int maxdiff_um_plexi = (int)(1000.0 * material::Tissue2Plexiglas(parent_->ui()->moddeglib_maxDParcoursLineEdit->text().toDouble()));

    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        auto degradeur_sets = parent_->repo()->GetBestDegradeurCombinations(des_um_plexi, maxdiff_um_plexi);
        if (degradeur_sets.empty()) {
            QMessageBox::warning(parent_, "Empty set", "No degradeurs found");
            return;
        }

        WriteDegradeursInTable(degradeur_sets, des_um_plexi);
    }
    catch (std::exception& exc) {
        qWarning() << QString("MTP::ListDegradeurCombinations An exception was thrown ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void LibraryPage::WriteDegradeursInTable(const std::vector<DegradeurSet>& deg_sets, const int& des_um_plexi) {
    parent_->ui()->moddeglib_degradeurProposalTableWidget->clearContents();
    for (int idx = 0; idx < std::min((int)deg_sets.size(), parent_->ui()->moddeglib_degradeurProposalTableWidget->rowCount()); ++idx) {
        parent_->ui()->moddeglib_degradeurProposalTableWidget->setItem(idx, DEG_ID, new QTableWidgetItem(deg_sets.at(idx).mm_plexis().join(", ")));
        parent_->ui()->moddeglib_degradeurProposalTableWidget->setItem(idx, DEG_DEGR, new QTableWidgetItem(QString::number(material::Plexiglas2Tissue((double)(deg_sets.at(idx).um_plexi_total()) / 1000.0), 'f', 2)));
        parent_->ui()->moddeglib_degradeurProposalTableWidget->setItem(idx, DEG_DELTADEGR, new QTableWidgetItem(QString::number(material::Plexiglas2Tissue((double)(deg_sets.at(idx).um_plexi_total() - des_um_plexi) / 1000.0), 'f', 2)));
    }
}

void LibraryPage::CalcRefSOBPCurve(const SOBPMaker& sobp_maker) {
    ref_sobp_graph_ = parent_->ui()->moddeglib_sobpCustomPlot->addGraph();
    ref_sobp_graph_->setPen(QPen(Qt::red));
    ref_sobp_graph_->setLineStyle(QCPGraph::lsLine);
    ref_sobp_graph_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::red, Qt::red, 5));

    auto mod_ref(parent_->repo()->GetModulateurForDossier(10000));
    auto deg_ref(parent_->repo()->GetDegradeurSetForDossier(10000));

    ref_sobp_curve_ = sobp_maker.GetSOBP(mod_ref, deg_ref);
    auto ref_depth_dose_vector(ref_sobp_curve_.Curve().DepthDoses());

    QVector<double> keys;
    QVector<double> values;
    for (DepthDose& depth_dose : ref_depth_dose_vector) {
        keys.push_back(depth_dose.depth());
        values.push_back(depth_dose.dose());
    }
    ref_sobp_graph_->setData(keys, values, false);
    ref_sobp_graph_->setVisible(true);
    ref_sobp_graph_->setName("SOBP REF");

    if (ref_zhalf_tracer) delete ref_zhalf_tracer;
    ref_zhalf_tracer = new QCPItemTracer(parent_->ui()->moddeglib_sobpCustomPlot);
    ref_zhalf_tracer->setGraph(ref_sobp_graph_);
    ref_zhalf_tracer->setGraphKey(ref_sobp_curve_.ZHalf());
    ref_zhalf_tracer->setInterpolating(true);
    ref_zhalf_tracer->setStyle(QCPItemTracer::tsCircle);
    ref_zhalf_tracer->setPen(QPen(Qt::green));
    ref_zhalf_tracer->setBrush(Qt::green);
    ref_zhalf_tracer->setSize(7);
    QObject::connect(parent_->ui()->moddeglib_ZhalfRefLineEdit, SIGNAL(editingFinished()), this, SLOT(UpdateZHalf()));
}

void LibraryPage::CalcActSOBPCurve(const SOBPMaker& sobp_maker, const Modulateur& mod, const DegradeurSet& deg) {
    QColor color = QColor("#3daee9");
    sobp_graph_ = parent_->ui()->moddeglib_sobpCustomPlot->addGraph();
    sobp_graph_->setPen(QPen(color));
    sobp_graph_->setLineStyle(QCPGraph::lsLine);
    sobp_graph_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, color, color, 5));

    sobp_curve_ = sobp_maker.GetSOBP(mod, deg);
    auto depth_dose_vector(sobp_curve_.Curve().DepthDoses());
    QVector<double> keys;
    QVector<double> values;
    for (DepthDose& depth_dose : depth_dose_vector) {
        keys.push_back(depth_dose.depth());
        values.push_back(depth_dose.dose());
    }
    sobp_graph_->setData(keys, values, false);
    sobp_graph_->setVisible(true);
    sobp_graph_->setName("SOBP ACT");

    if (zhalf_tracer) delete zhalf_tracer;
    zhalf_tracer = new QCPItemTracer(parent_->ui()->moddeglib_sobpCustomPlot);
    zhalf_tracer->setGraph(sobp_graph_);
    zhalf_tracer->setGraphKey(sobp_curve_.ZHalf());
    zhalf_tracer->setInterpolating(true);
    zhalf_tracer->setStyle(QCPItemTracer::tsCircle);
    zhalf_tracer->setPen(QPen(Qt::green));
    zhalf_tracer->setBrush(Qt::green);
    zhalf_tracer->setSize(7);
    QObject::connect(parent_->ui()->moddeglib_ZhalfActLineEdit, SIGNAL(editingFinished()), this, SLOT(UpdateZHalf()));
}

Modulateur LibraryPage::GetActModulateur() {
    bool mod_id_ok(false);
    const int mod_act_id(parent_->ui()->moddeglib_selectModLineEdit->text().toInt(&mod_id_ok));
    if (!mod_id_ok) {
        throw std::runtime_error("Non valid modulator id");
    }
    return parent_->repo()->GetModulateur(mod_act_id);
}

DegradeurSet LibraryPage::GetActDegradeur() {
    const QString deg_string(parent_->ui()->moddeglib_selectDegLineEdit->text().trimmed());
    const QStringList deg_ids(deg_string.split(","));
    if (deg_ids.size() > 2) {
        throw std::runtime_error("Non valid number of degradeurs");
    }
    for (auto id : deg_ids) {
        bool deg_id_ok(false);
        id.toDouble(&deg_id_ok);
        if (!deg_id_ok) {
           throw std::runtime_error("Non valid degradeur id");
        }
    }

    DegradeurSet deg_set;
    if (deg_ids.size() == 1) {
        deg_set = DegradeurSet(parent_->repo()->GetDegradeur((int)(std::round(1000*deg_ids.at(0).toDouble()))));
    } else {
        deg_set = DegradeurSet(parent_->repo()->GetDegradeur((int)(std::round(1000*deg_ids.at(0).toDouble()))),
                               parent_->repo()->GetDegradeur((int)(std::round(1000*deg_ids.at(1).toDouble()))));
    }
    return deg_set;
}

void LibraryPage::CalcIndCurves() {
    auto individual_weighted_curves(sobp_curve_.IndividualWeightedCurves());
    for (DepthDoseCurve& bp_graph : individual_weighted_curves) {
        auto graph = AddBPGraph();
        QVector<double> keys;
        QVector<double> values;
        for (DepthDose& depth_dose : bp_graph.DepthDoses()) {
            keys.push_back(depth_dose.depth());
            values.push_back(depth_dose.dose());
        }
        graph->setData(keys, values, false);
        graph->setVisible(true);
        graph->removeFromLegend(parent_->ui()->moddeglib_sobpCustomPlot->legend);
    }
}

void LibraryPage::CalculateSOBPCurves() {
    try {
        parent_->ui()->moddeglib_sobpCustomPlot->clearPlottables();

        if (!parent_->repo()->CheckConnection()) { return; }
        auto mcnpx_data = parent_->repo()->GetMCNPXData();

        SOBPMaker sobp_maker(mcnpx_data.depth_dose_curves(),
                             mcnpx_data.monitor_units(),
                             mcnpx_data.um_plexi());

        auto mod_act = GetActModulateur();
        auto deg_act = GetActDegradeur();
        qDebug() << "LibraryPage::CalculateSOBPCurves Modulateur = " << mod_act.id() << " Degradeur(s) = " << deg_act.ids().join(", ");
        CalcRefSOBPCurve(sobp_maker);
        CalcActSOBPCurve(sobp_maker, mod_act, deg_act);
        CalcIndCurves();

        parent_->ui()->moddeglib_sobpCustomPlot->rescaleAxes(true);
        parent_->ui()->moddeglib_sobpCustomPlot->yAxis->setRangeUpper(1.2 * parent_->ui()->moddeglib_sobpCustomPlot->yAxis->range().upper);

        parent_->ui()->moddeglib_sobpCustomPlot->legend->setVisible(true);
        parent_->ui()->moddeglib_sobpCustomPlot->legend->setFont(QFont("Helvetica", 9));
        parent_->ui()->moddeglib_sobpCustomPlot->legend->setRowSpacing(-3);

        parent_->ui()->moddeglib_sobpCustomPlot->replot();

        parent_->ui()->moddeglib_ZhalfActLineEdit->setText(QString::number(sobp_curve_.ZHalf(), 'f', 2));
        parent_->ui()->moddeglib_DMUActLineEdit->setText(QString::number(sobp_curve_.DMUZHalf(), 'f', 4));
        parent_->ui()->moddeglib_ZhalfRefLineEdit->setText(QString::number(ref_sobp_curve_.ZHalf(), 'f', 2));
        parent_->ui()->moddeglib_DMURefLineEdit->setText(QString::number(ref_sobp_curve_.DMUZHalf(), 'f', 4));
        auto defaults(parent_->repo()->GetDefaults());
        const double debit_mcnpx(defaults.GetDRef() * sobp_curve_.DMUZHalf() / ref_sobp_curve_.DMUZHalf());
        const double debit_debroca(util::DebitDebroca(defaults.GetDRef(), mod_act , deg_act));
        parent_->ui()->moddeglib_debitDEBROCALineEdit->setText(QString::number(debit_debroca, 'f', 4));
        parent_->ui()->moddeglib_debitMCNPXLineEdit->setText(QString::number(debit_mcnpx, 'f', 4));
        parent_->ui()->moddeglib_ratioDebitLineEdit->setText(QString::number(debit_mcnpx / debit_debroca, 'f', 2));
    }
    catch (std::exception& exc) {
        qWarning() << QString("LibraryPage::CalculateSOBPCurves Exception catched ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
    catch (...) {
        QMessageBox::warning(parent_, tr("MTD"), QString("Unknown exception caught"));
    }
}

void LibraryPage::UpdateZHalf() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        const double zhalf(parent_->ui()->moddeglib_ZhalfActLineEdit->text().toDouble());
        if (zhalf > parent_->ui()->moddeglib_sobpCustomPlot->xAxis->range().maxRange ||
                zhalf < parent_->ui()->moddeglib_sobpCustomPlot->xAxis->range().minRange) {
            QMessageBox::warning(parent_, QString("MTD"), QString("Value out of range"));
            return;
        }
        const double dmu_zhalf(sobp_curve_.DMU(zhalf));
        const double ref_zhalf(parent_->ui()->moddeglib_ZhalfRefLineEdit->text().toDouble());
        const double ref_dmu_zhalf(ref_sobp_curve_.DMU(ref_zhalf));
        zhalf_tracer->setGraphKey(zhalf);
        ref_zhalf_tracer->setGraphKey(ref_zhalf);
        parent_->ui()->moddeglib_DMUActLineEdit->setText(QString::number(dmu_zhalf, 'f', 4));
        parent_->ui()->moddeglib_DMURefLineEdit->setText(QString::number(ref_dmu_zhalf, 'f', 4));
        auto defaults = parent_->repo()->GetDefaults();
        const double debit_mcnpx(defaults.GetDRef() * dmu_zhalf / ref_dmu_zhalf);
        const double debit_debroca(parent_->ui()->moddeglib_debitDEBROCALineEdit->text().toDouble());
        parent_->ui()->moddeglib_debitMCNPXLineEdit->setText(QString::number(debit_mcnpx, 'f', 4));
        parent_->ui()->moddeglib_ratioDebitLineEdit->setText(QString::number(debit_mcnpx / debit_debroca, 'f', 4));

        parent_->ui()->moddeglib_sobpCustomPlot->rescaleAxes(true);
        parent_->ui()->moddeglib_sobpCustomPlot->yAxis->setRangeUpper(1.2 * parent_->ui()->moddeglib_sobpCustomPlot->yAxis->range().upper);
        parent_->ui()->moddeglib_sobpCustomPlot->replot();
    }
    catch (std::exception& exc) {
        qWarning() << QString("LibraryPage::UpdateZHalf Exception catched ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

QCPGraph* LibraryPage::AddBPGraph() {
    QCPGraph* graph = parent_->ui()->moddeglib_sobpCustomPlot->addGraph();
    graph->setPen(QPen(Qt::green));
    graph->setLineStyle(QCPGraph::lsLine);
    graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::green, Qt::green, 5));
    return graph;
}

void LibraryPage::mousePress()
{
    parent_->ui()->moddeglib_sobpCustomPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void LibraryPage::mouseWheel()
{
    parent_->ui()->moddeglib_sobpCustomPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void LibraryPage::SelectModulateur(int table_row) {
    if (!parent_->ui()->moddeglib_modulatorProposalTableWidget->item(table_row, (int)MODCOL::MOD_ID)) {
        return;
    }
    parent_->ui()->moddeglib_selectDegLineEdit->clear();
    parent_->ui()->moddeglib_selectModLineEdit->setText(parent_->ui()->moddeglib_modulatorProposalTableWidget->item(table_row, (int)MODCOL::MOD_ID)->text());
}

void LibraryPage::SelectDegradeur(int table_row) {
    if (!parent_->ui()->moddeglib_degradeurProposalTableWidget->item(table_row, (int)DEGCOL::DEG_ID)) {
        return;
    }
    parent_->ui()->moddeglib_selectDegLineEdit->setText(parent_->ui()->moddeglib_degradeurProposalTableWidget->item(table_row, (int)DEGCOL::DEG_ID)->text());
}

void LibraryPage::AssociateToDossier() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        auto mod_act = GetActModulateur();
        auto deg_act = GetActDegradeur();
        const double debit_mcnpx = parent_->ui()->moddeglib_debitMCNPXLineEdit->text().toDouble();
        AssociateDialog dialog(parent_, parent_->repo(), deg_act, mod_act, debit_mcnpx);
        dialog.exec();
    }
    catch (std::exception& exc) {
        qWarning() << QString("LibraryPage::AssociateToDossier Exception catched ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
    catch (...) {
        QMessageBox::warning(parent_, tr("MTP"), QString("An unknown problem occurred"));
    }
}

