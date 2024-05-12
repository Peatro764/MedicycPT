#include "AddSeanceDialog.h"
#include "ui_AddSeanceDialog.h"

AddSeanceDialog::AddSeanceDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::AddSeanceDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle("Ajouter une séance");

    ui_->doseLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{0,}\\.{0,1}\\d{0,}"), ui_->doseLineEdit));

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->updatePushButton, SIGNAL(clicked()), this, SLOT(AddSeance()));
}

void AddSeanceDialog::AddSeance() {
    if (ui_->doseLineEdit->text().isEmpty() || ui_->doseLineEdit->text().isNull()) {
        QMessageBox::warning(this, "MTP", "Veuillez entrer une dose valide");
        return;
    }
    try {
        const double dose(ui_->doseLineEdit->text().toDouble());
        if (dose <= 0.0) {
            QMessageBox::warning(this, "MTP", "veuillez entrer une dose supérieure à 0");
            return;
        }
        repo_->AddSeanceToExistingTreatment(dossier_, dose);
        accept();
    }
    catch (std::exception& exc) {
        qWarning() << "AddSeanceDialog Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}
