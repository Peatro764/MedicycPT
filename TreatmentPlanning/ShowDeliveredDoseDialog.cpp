#include "ShowDeliveredDoseDialog.h"
#include "ui_ShowDeliveredDoseDialog.h"

#include <vector>

#include "Calc.h"
#include "Algorithms.h"
#include "SeanceProgressGraph.h"


ShowDeliveredDoseDialog::ShowDeliveredDoseDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ShowDeliveredDoseDialog),
    repo_(repo),
    dossier_(dossier),
    seance_dose_graph_(nullptr)
{
    ui_->setupUi(this);
    setWindowTitle("Delivered dose");

    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

ShowDeliveredDoseDialog::~ShowDeliveredDoseDialog() {
    if (seance_dose_graph_) delete seance_dose_graph_;
}

void ShowDeliveredDoseDialog::InitGraph() {
    seance_dose_graph_ = new SeanceDoseGraph(ui_->seanceDoseChartView);
    seance_dose_graph_->DisableDropShadowEffect();

    try {
        Treatment treatment(repo_->GetTreatment(dossier_));
        seance_dose_graph_->SetData(treatment.GetAllSeances());
        QObject::connect(seance_dose_graph_, SIGNAL(ShowSeanceInfoReq(int)), this, SLOT(ShowSeanceInfo(int)));
    }
    catch (std::exception const &exc) {
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

void ShowDeliveredDoseDialog::ShowSeanceInfo(int seance_idx) {
    try {
        if (!seance_dose_graph_) {
            return;
        }
       Treatment treatment(repo_->GetTreatment(dossier_));
       auto seance = treatment.GetSeance(seance_idx);
       QDialog *dialog = new QDialog(this);
       QGridLayout *grid_layout = new QGridLayout;
       int row_nmb = 0;
       QString top_info("");
       top_info.append("Dose planifiée:   " + QString::number(seance.GetDosePrescribed(), 'f', 3) + " Gy\n");
       top_info.append("Dose delivree: " + QString::number(seance.GetDoseDelivered(), 'f', 3) + " Gy");
       grid_layout->addWidget(new QLabel(top_info), row_nmb++, 0);

       std::vector<SeanceProgressGraph*> progress_graphs;

       auto records = seance.GetSeanceRecords();
       if (records.empty()) {
           grid_layout->addWidget(new QLabel("No delivered beams"), row_nmb++, 0);
       } else {
           int rec_nmb(1);
           for (auto rec : records) {
               QString text("");
               text.append("\nFaisceau:          " + QString::number(rec_nmb++) + "\n");
               text.append("Horodatage:          " + rec.GetTimestamp().toString() + "\n");
               text.append("Durée:               " + QString::number(rec.GetTotalDuration(), 'f', 1) + " s\n");
               text.append("I.Stripper:          " + QString::number(rec.GetIStripper(), 'f', 0) + " nA\n");
               text.append("I.CF9:               " + QString::number(rec.GetICF9(), 'f', 0) + " nA\n");
               text.append("I.Chambre1:          " + QString::number(calc::Mean(rec.GetIChambre1()), 'f', 0) + " nA\n");
               text.append("I.Chambre2:          " + QString::number(calc::Mean(rec.GetIChambre2()), 'f', 0) + " nA\n");
               text.append("Débit:               " + QString::number(rec.GetDebit(), 'f', 4) + " cGy/UM\n");
               text.append("UM Prevu:            " + QString::number(rec.GetUMPrevu()) + "\n");
               text.append("UM Délivrée 1:       " + QString::number(rec.GetTotalUM1Delivered()) + "\n");
               text.append("UM Délivrée 2:       " + QString::number(rec.GetTotalUM2Delivered()) + "\n");
               text.append("Dose délivrée (Gy):  " + QString::number(rec.GetDoseDelivered(), 'f', 3) + "\n");
               text.append("Dose estimée (Gy):   " + QString::number(rec.GetDoseEstimated(), 'f', 3) + "\n");
               grid_layout->addWidget(new QLabel(text), row_nmb, 0, Qt::AlignTop);

               QtCharts::QChartView *chart_view = new QtCharts::QChartView;
               chart_view->setMinimumHeight(300);
               chart_view->setMinimumWidth(500);
               SeanceProgressGraph *progress_graph = new SeanceProgressGraph(chart_view, false, false);
               progress_graphs.push_back(progress_graph);
               progress_graph->SetData(rec);
               grid_layout->addWidget(chart_view, row_nmb, 1, Qt::AlignTop);
               row_nmb++;
           }
       }
       QPushButton *b = new QPushButton;
       b->setText("Close");
       b->setMaximumWidth(100);
       grid_layout->addWidget(b, row_nmb++, 1, Qt::AlignRight);
       QObject::connect(b, SIGNAL(pressed()), dialog, SLOT(accept()));
       dialog->setLayout(grid_layout);
       dialog->exec();
       for (SeanceProgressGraph *p : progress_graphs) {
           delete p;
       }
    }
    catch (std::exception const& exc) {
        qWarning() << QString("ShowDeliveredDoseDialog::ShowSeanceInfo Exception thrown: ") + exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}


