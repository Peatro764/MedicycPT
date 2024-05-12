#include "AssociateDialog.h"
#include "ui_AssociateDialog.h"

AssociateDialog::AssociateDialog(QWidget *parent, PTRepo* repo,
                                 const DegradeurSet& deg_set, const Modulateur& mod, double mcnpx_debit)
    : QDialog(parent),
      ui_(new Ui::AssociateDialog),
      repo_(repo),
      deg_set_(deg_set),
      mod_(mod),
      mcnpx_debit_(mcnpx_debit)
{
    ui_->setupUi(this);
    setWindowTitle("Associer au dossier");

    ui_->dossierLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{0,}"), ui_->dossierLineEdit));
    ui_->modulateurLabel->setText(mod_.id());
    ui_->degradeurLabel->setText(deg_set_.mm_plexis().join(", "));
    ui_->debitLabel->setText(QString::number(mcnpx_debit_, 'f', 5));

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->associatePushButton, SIGNAL(clicked()), this, SLOT(Associate()));
}

void AssociateDialog::Associate() {
    if (ui_->dossierLineEdit->text().isEmpty() || ui_->dossierLineEdit->text().isNull()) {
        QMessageBox::warning(this, "MTP", "Please enter a dossier");
        return;
    }
    try {
        const int dossier(ui_->dossierLineEdit->text().toInt());
        if (!repo_->DossierExist(dossier)) {
            QString msg = "Dossier " + QString::number(dossier) + " does not exist";
            throw std::runtime_error(msg.toStdString().c_str());
        }

        const int pri_degradeur_id(deg_set_.ids().at(0).toInt());
        const int sec_degradeur_id(deg_set_.single() ? -1 : deg_set_.ids().at(1).toInt());
        repo_->ChangeDegradeurSet(dossier, pri_degradeur_id, sec_degradeur_id);
        repo_->ChangeModulateur(dossier, mod_.id().toInt());
        repo_->SaveMonteCarloDosimetry(dossier, mcnpx_debit_);
        accept();
    }
    catch (std::exception& exc) {
        qWarning() << "AssociateDialog::Associate Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}
