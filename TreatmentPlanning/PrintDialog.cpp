#include "PrintDialog.h"
#include "ui_PrintDialog.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QDateTime>
#include <QThread>

#include "Dosimetry.h"
#include "Calc.h"

PrintDialog::PrintDialog(QWidget *parent, PTRepo* pt_repo, QARepo* qa_repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::PrintDialog),
    pt_repo_(pt_repo),
    qa_repo_(qa_repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Print Dossier"));
    SetupConnections();
    CreateDialogs();
    printer_.setPageSize(QPrinter::A4);
    printer_.setFullPage(false);
}

PrintDialog::~PrintDialog()
{
    delete ui_;
}

void PrintDialog::SetupConnections() {
//    QObject::connect(ui_->showDosimetryPushButton, SIGNAL(clicked()) , this, SLOT(LaunchDosimetryDialog()));
//    QObject::connect(ui_->showTreatmentPushButton, SIGNAL(clicked()), this, SLOT(LaunchTreatmentDialog()));
//    QObject::connect(ui_->showCompensateurPushButton, SIGNAL(clicked()), this, SLOT(LaunchCompensateurDialog()));
//    QObject::connect(ui_->showCollimateurPushButton, SIGNAL(clicked()), this, SLOT(LaunchCollimatorDialog()));
//    QObject::connect(ui_->showControleDosePushButton, SIGNAL(clicked()), this, SLOT(LaunchControleDoseDialog()));
//    QObject::connect(ui_->showControleQualitePushButton, SIGNAL(clicked()), this, SLOT(LaunchControleQualiteDialog()));
    QObject::connect(ui_->printPushButton, SIGNAL(clicked()), this, SLOT(PrintSelected()));
    QObject::connect(ui_->closePushButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void PrintDialog::CreateDialogs() {
    try {
        qDebug() << "Create SOBPPrintDialog";
        sobp_dialog_ = new SOBPPrintDialog(this, qa_repo_);
        sobp_dialog_->Load(dossier_);
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create SOBP dialog: " << exc.what();
        sobp_dialog_ = nullptr;
        ui_->showSOBPPushButton->setEnabled(false);
    }

    try {
        qDebug() << "Create ControleDeLaDose";
        controle_dose_dialog_ = new ControleDeLaDosePrintDialog(this, pt_repo_, dossier_);
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create controledeladose dialog: " << exc.what();
        controle_dose_dialog_ = nullptr;
        ui_->showControleDosePushButton->setEnabled(false);
    }

    try {
        qDebug() << "Create ControlQialite";
        controle_qualite_dialog_ = new ControleQualitePrintDialog(this, pt_repo_, dossier_);
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create controlequalite dialog: " << exc.what();
        controle_qualite_dialog_ = nullptr;
        ui_->showControleQualitePushButton->setEnabled(false);
    }

    try {
        qDebug() << "Create PatientDebit";
        patientDebit_dialog_ = new PatientDebitPrintDialog(this, pt_repo_);
        patientDebit_dialog_->Load(dossier_);

    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create patient debit dialog: " << exc.what();
        patientDebit_dialog_ = nullptr;
        ui_->showDebitMesurePushButton->setEnabled(false);
    }

    try {
        qDebug() << "Create TreatmentDialog";
        treatment_dialog_ = new TreatmentPrintDialog(this, pt_repo_, dossier_);

    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create treatment dialog: " << exc.what();
        treatment_dialog_ = nullptr;
        ui_->showTreatmentPushButton->setEnabled(false);
    }

    try {
        qDebug() << "Create Compensator";
        compensateur_dialog_ = nullptr;
        Compensateur comp(pt_repo_->GetCompensateur(dossier_));
        Patient patient(pt_repo_->GetPatient(dossier_));
        compensateur_dialog_ = new CompensateurTransparantDialog(this, comp, patient);
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create compensateur dialog: " << exc.what();
        ui_->showCompensateurPushButton->setEnabled(false);
    }

    try {
        qDebug() << "Create Collimator";
        collimator_dialog_ = nullptr;
        collimator_dialog_ = new CollimatorPrintDialog(this, pt_repo_, dossier_);
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create collimator dialog: " << exc.what();
        ui_->showCollimateurPushButton->setEnabled(false);
    }
}

void PrintDialog::LaunchSOBPDialog() {
    sobp_dialog_->exec();
}

void PrintDialog::LaunchDebitMesureDialog() {
    patientDebit_dialog_->exec();
}

void PrintDialog::LaunchTreatmentDialog() {
    treatment_dialog_->exec();
}

void PrintDialog::LaunchControleDoseDialog() {
    controle_dose_dialog_->exec();
}

void PrintDialog::LaunchControleQualiteDialog() {
    controle_qualite_dialog_->exec();
}

void PrintDialog::LaunchCompensateurDialog() {
    compensateur_dialog_->exec();
}

void PrintDialog::LaunchCollimatorDialog() {
    collimator_dialog_->exec();
}

void PrintDialog::PrintSelected() {
    if (!ui_->showTreatmentPushButton->isChecked() &&
            !ui_->showCompensateurPushButton->isChecked() &&
            !ui_->showSOBPPushButton->isChecked() &&
            !ui_->showCollimateurPushButton->isChecked() &&
            !ui_->showControleDosePushButton->isChecked() &&
            !ui_->showDebitMesurePushButton->isCheckable() &&
            !ui_->showControleQualitePushButton->isChecked()) {
        QMessageBox::warning(this, tr("MTP"), "No ducuments selected");
        return;
    }

    QPrintDialog *dialog = new QPrintDialog(&printer_, this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    try {
        QVector<QWidget*> pages;
        QVector<QPrinter::Orientation> orientations;
        if (ui_->showDebitMesurePushButton->isChecked()) { pages.push_back(patientDebit_dialog_->Widget());
                                                           orientations.push_back(patientDebit_dialog_->Orientation()); }//   patientDebit_dialog_->Print(&printer_); }
        if (ui_->showTreatmentPushButton->isChecked()) { pages.push_back(treatment_dialog_->Widget());
                                                         orientations.push_back(treatment_dialog_->Orientation()); }//   treatment_dialog_->Print(&printer_); }
        if (ui_->showSOBPPushButton->isChecked()) { pages.push_back(sobp_dialog_->Widget());
                                                    orientations.push_back(sobp_dialog_->Orientation()); }//  sobp_dialog_->Print(&printer_); }
        if (ui_->showCompensateurPushButton->isChecked()) { pages.push_back(compensateur_dialog_->Widget());
                                                            orientations.push_back(compensateur_dialog_->Orientation()); }//  compensateur_dialog_->Print(&printer_); }
        if (ui_->showCollimateurPushButton->isChecked()) { pages.push_back(collimator_dialog_->Widget());
                                                           orientations.push_back(collimator_dialog_->Orientation()); }//  collimator_dialog_->Print(&printer_); }
        if (ui_->showControleDosePushButton->isChecked()) { pages.push_back(controle_dose_dialog_->Widget());
                                                            orientations.push_back(controle_dose_dialog_->Orientation()); }// controle_dose_dialog_->Print(&printer_); }
        if (ui_->showControleQualitePushButton->isChecked()) { pages.push_back(controle_qualite_dialog_->Widget());
                                                               orientations.push_back(controle_qualite_dialog_->Orientation()); }//  controle_qualite_dialog_->Print(&printer_); }

        if (pages.empty()) {
            QMessageBox::warning(this, tr("MTP"), "No documents selected");
            return;
        }
        if (pages.size() != orientations.size()) {
            QMessageBox::warning(this, tr("MTP"), "Internal error 666");
            return;
        }

        for (int i = 0; i < pages.size(); ++i) {
            printer_.setOrientation(orientations.at(i));
            QCPPainter painter(&printer_);
            QWidget *w = pages.at(i);
            QRectF pageRect = printer_.pageRect(QPrinter::DevicePixel);
            int plotWidth = w->width();
            int plotHeight = w->height();
            double w_scale = pageRect.width() / (double)plotWidth;
            double h_scale = pageRect.height() / (double)plotHeight;

            painter.scale(w_scale, h_scale);
            w->render(&painter);
            painter.resetTransform();
            if(w != pages.last()) {
               printer_.newPage();
            }
            painter.end();
        }
    }
    catch (std::exception const& exc) {
        qWarning() << "PrintDialog::Print catched an exception: " << exc.what();
        QMessageBox::warning(this, tr("MTP"), "Printing failed");
    }
}

