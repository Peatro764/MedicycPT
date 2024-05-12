#include "PrintDialog.h"
#include "ui_PrintDialog.h"

#include <QPrintDialog>
#include <QPrinter>
#include <QDateTime>

#include "Calc.h"

PrintDialog::PrintDialog(QWidget *parent, std::shared_ptr<QARepo> qa_repo, std::shared_ptr<PTRepo> pt_repo) :
    QDialog(parent),
    ui_(new Ui::PrintDialog),
    qa_repo_(qa_repo), pt_repo_(pt_repo)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Print QA"));
    SetupConnections();
    printer_.setPageSize(QPrinter::A4);
    CreateDialogs();
}

PrintDialog::~PrintDialog()
{
    delete ui_;
}

void PrintDialog::SetupConnections() {
    QObject::connect(ui_->pushButton_print, SIGNAL(clicked()), this, SLOT(PrintSelected()));
    QObject::connect(ui_->pushButton_quitte, SIGNAL(clicked()), this, SLOT(reject()));
}

void PrintDialog::CreateDialogs() {
    try {
        profile_dialog_ =  std::unique_ptr<ProfilePrintDialog>(new ProfilePrintDialog(this, qa_repo_));
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create profileprintdialog: " << exc.what();
        ui_->pushButton_showProfile->setEnabled(false);
    }

    try {
        braggPeak_dialog_ =  std::unique_ptr<BraggPeakPrintDialog>(new BraggPeakPrintDialog(this, qa_repo_));
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create braggpeakprintdialog: " << exc.what();
        ui_->pushButton_showPicDeBragg->setEnabled(false);
    }

    try {
        sobp_dialog_ =  std::unique_ptr<SOBPPrintDialog>(new SOBPPrintDialog(this, qa_repo_.get()));
        sobp_dialog_->LoadLastSOBP();
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create sobpprintdialog: " << exc.what();
        ui_->pushButton_showSOBP->setEnabled(false);
    }

    try {
        topDeJour_dialog_ =  std::unique_ptr<TopDeJourPrintDialog>(new TopDeJourPrintDialog(this, pt_repo_));
        topDeJour_dialog_->Load();
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create topdejourprintdialog: " << exc.what();
        ui_->pushButton_showTopDeJour->setEnabled(false);
    }

    try {
        patientDebit_dialog_ =  std::unique_ptr<PatientDebitPrintDialog>(new PatientDebitPrintDialog(this, pt_repo_.get()));
        patientDebit_dialog_->LoadLastDossier();
    }
    catch (std::exception& exc) {
        qWarning() << "PrintDialog::CreateDialogs Could not create patientdebitprintdialog: " << exc.what();
        ui_->pushButton_showDebitPatient->setEnabled(false);
    }

    ui_->pushButton_showScan3D->setEnabled(false);
}

void PrintDialog::LaunchTopDeJourDialog() {
    try {
        topDeJour_dialog_->exec();
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, tr("QA"), "Failed retrieving top de jour data");
    }
}

void PrintDialog::LaunchDebitPatientDialog() {
    try {
        patientDebit_dialog_->exec();
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, tr("QA"), "Failed retrieving patient debit data");
    }
}

void PrintDialog::Launch3DScanDialog() {
//    controle_dose_dialog_->exec();
}

void PrintDialog::LaunchProfileDialog() {
    profile_dialog_->exec();
}

void PrintDialog::LaunchPicDeBraggDialog() {
    braggPeak_dialog_->exec();
}

void PrintDialog::LaunchSOBPDialog()  {
    sobp_dialog_->exec();
}

void PrintDialog::PrintSelected() {
    if (!ui_->pushButton_showDebitPatient->isChecked() &&
            !ui_->pushButton_showTopDeJour->isChecked() &&
            !ui_->pushButton_showPicDeBragg->isChecked() &&
            !ui_->pushButton_showProfile->isChecked() &&
            !ui_->pushButton_showScan3D->isChecked() &&
            !ui_->pushButton_showSOBP->isChecked()) {
        QMessageBox::warning(this, tr("QA"), "No ducuments selected");
        return;
    }

    QPrintDialog *dialog = new QPrintDialog(&printer_, this);
    if (dialog->exec() != QDialog::Accepted)
        return;

    try {
        if (ui_->pushButton_showDebitPatient->isChecked()) { patientDebit_dialog_->Print(&printer_); }
        if (ui_->pushButton_showTopDeJour->isChecked()) { topDeJour_dialog_->Print(&printer_); }
        if (ui_->pushButton_showPicDeBragg->isChecked()) { braggPeak_dialog_->Print(&printer_); }
        if (ui_->pushButton_showProfile->isChecked()) { profile_dialog_->Print(&printer_); }
//        if (ui_->radioButton_scan3D->isChecked()) { controle_dose_dialog_->Print(&printer); }
        if (ui_->pushButton_showSOBP->isChecked()) { sobp_dialog_->Print(&printer_); }
    }
    catch (std::exception const& exc) {
        qWarning() << "PrintDialog::Print catched an exception: " << exc.what();
        QMessageBox::warning(this, tr("MTP"), "Printing failed");
    }
}

