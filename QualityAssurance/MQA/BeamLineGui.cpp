#include "BeamLineGui.h"

#include <QMessageBox>

#include "ui_MQA.h"
#include "MQA.h"

BeamLineGui::BeamLineGui(MQA *parent, std::shared_ptr<PTRepo> repo) :
    GuiBase(parent),
    repo_(repo)
{
    ConnectSignals();
    InitDateSelector();
    SetupGraphs();
}

BeamLineGui::~BeamLineGui() {}

void BeamLineGui::ConnectSignals() {
    QObject::connect(parent_->ui()->beamline_pushButton_results_updatePlot, &QPushButton::clicked, this, &BeamLineGui::FillTransmissionPageGraphs);
}

void BeamLineGui::Configure() {
    FillTransmissionPageGraphs();
}

void BeamLineGui::SetupGraphs() {
    transmission_graph_ = std::shared_ptr<TimedStampedDataGraph>(new TimedStampedDataGraph(parent_->ui()->beamline_customPlot_transmission));
    transmission_graph_->SetYAxisTitle("[%]");
}

void BeamLineGui::InitDateSelector() {
    parent_->ui()->beamline_dateEdit_from->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->beamline_dateEdit_to->setDisplayFormat("yyyy.MM.dd");
    parent_->ui()->beamline_dateEdit_from->setCurrentSection(QDateTimeEdit::MonthSection);
    parent_->ui()->beamline_dateEdit_to->setCurrentSection(QDateTimeEdit::MonthSection);
    QDateTime current_date(QDateTime::currentDateTime());
    parent_->ui()->beamline_dateEdit_from->setDate(current_date.date().addDays(-365));
    parent_->ui()->beamline_dateEdit_to->setDate(current_date.date());
    parent_->ui()->beamline_dateEdit_from->setDate(current_date.date().addDays(-365*2));
    parent_->ui()->beamline_dateEdit_to->setDate(current_date.date());
}

void BeamLineGui::FillTransmissionPageGraphs() {
    try {
        QDate from_date(parent_->ui()->beamline_dateEdit_from->date());
        QDate to_date(parent_->ui()->beamline_dateEdit_to->date());
        qDebug() << "BeamLineGui::FillTransmissionPageGraphs " << from_date << " " << to_date;

        transmission_graph_->Clear();
        transmission_graph_->RemoveAll();
        transmission_graph_->Register(transmission_graph_name_);
        TimedStampedDataSeries data = repo_->GetBeamTransmissionSeries(from_date, to_date);
        transmission_graph_->SetData(transmission_graph_name_, data);
        transmission_graph_->SetTimeRange(from_date, to_date);
    }
    catch(std::exception& exc) {
        qWarning() << "BeamLineGui::FillTransmissionPageGraphs Exception thrown: " << exc.what();
    }
}

void BeamLineGui::CleanUp() {
}
