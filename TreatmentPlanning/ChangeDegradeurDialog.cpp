#include "ChangeDegradeurDialog.h"
#include "ui_ChangeDegradeurDialog.h"

ChangeDegradeurDialog::ChangeDegradeurDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::ChangeDegradeurDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);

    FillDegradeurs();
    SetActiveDegradeur();

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->updatePushButton, SIGNAL(clicked()), this, SLOT(ChangeDegradeur()));
    setWindowTitle("Change degradeur(s)");
}

void ChangeDegradeurDialog::FillDegradeurs() {
    try {
        ui_->degradeur2ComboBox->addItem("None", "-1");
        auto degradeurs(repo_->GetAllAvailableDegradeurs());
        for (auto degr : degradeurs) {
            ui_->degradeur1ComboBox->addItem(QString::number(degr.mm_plexi(), 'f', 1) , degr.id());
            ui_->degradeur2ComboBox->addItem(QString::number(degr.mm_plexi(), 'f', 1), degr.id());
        }
        ui_->degradeur1ComboBox->setCurrentIndex(0);
        ui_->degradeur2ComboBox->setCurrentIndex(0);
    }
    catch (std::exception& exc) {
        qWarning() << "ChangeDegradeurDialog::FillDegradeurs Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

void ChangeDegradeurDialog::SetActiveDegradeur() {
    try {
        auto degradeur_set(repo_->GetDegradeurSetForDossier(dossier_));
        ui_->degradeur1ComboBox->setCurrentText(degradeur_set.mm_plexis().at(0));
        if (degradeur_set.ids().size() == 2) {
            ui_->degradeur2ComboBox->setCurrentText(degradeur_set.mm_plexis().at(1));
        } else {
            ui_->degradeur2ComboBox->setCurrentText(QString("None"));
        }
    }
    catch (std::exception& exc) {
        qWarning() << "ChangeDegradeurDialog::SetActiveDegradeur Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

void ChangeDegradeurDialog::ChangeDegradeur() {
    try {
        const DegradeurSet degradeur_set(GetSelectedDegradeurSet());
        const int pri_degradeur_id(degradeur_set.ids().at(0).toInt());
        const int sec_degradeur_id(degradeur_set.single() ? -1 : degradeur_set.ids().at(1).toInt());
        repo_->ChangeDegradeurSet(dossier_, pri_degradeur_id, sec_degradeur_id);
        accept();
    }
    catch (std::exception& exc) {
        qWarning() << "ChangeDegradeurDialog::ChangeDegradeur Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}


DegradeurSet ChangeDegradeurDialog::GetSelectedDegradeurSet() {
    bool ok_pri(false);
    bool ok_sec(false);
    const auto pri_degradeur_id(ui_->degradeur1ComboBox->currentData().toInt(&ok_pri));
    const int sec_degradeur_id(ui_->degradeur2ComboBox->currentData().toInt(&ok_sec));
    if (!ok_pri || !ok_sec) {
        throw std::runtime_error("Not a valid degradeur id");
    }
    if (sec_degradeur_id < 0) {
        return DegradeurSet(repo_->GetDegradeur(pri_degradeur_id));
    } else {
        return DegradeurSet(repo_->GetDegradeur(pri_degradeur_id),
                            repo_->GetDegradeur(sec_degradeur_id));
    }
}
