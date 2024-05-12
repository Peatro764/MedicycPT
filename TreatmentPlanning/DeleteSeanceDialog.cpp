#include "DeleteSeanceDialog.h"
#include "ui_DeleteSeanceDialog.h"

#include "Calc.h"

DeleteSeanceDialog::DeleteSeanceDialog(QWidget *parent, PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::DeleteSeanceDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle("Delete Seance");

    FillSeanceComboBox();

    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->deletePushButton, SIGNAL(clicked()), this, SLOT(DeleteSeance()));
}

void DeleteSeanceDialog::DeleteSeance() {
    try {
        QString seance(ui_->seanceComboBox->currentText());
        int seance_id(ui_->seanceComboBox->currentData().toInt());
        qDebug() << "DeleteSeanceDialog::DeleteSeance Want to delete seance " << seance << " having db id " << seance_id;

        QMessageBox msg_box;
        msg_box.setText("Delete seance: " + seance + "?");
        msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg_box.setDefaultButton(QMessageBox::No);
        int ret = msg_box.exec();
        switch (ret) {
        case QMessageBox::Yes:
            repo_->DeleteSeance(dossier_, seance_id);
            accept();
            break;
        case QMessageBox::No:
            break;
        default:
            qWarning() << "DeleteSeanceDialog::DeleteSeance Not covered case";
            break;
        }
    }
    catch (std::exception& exc) {
        qWarning() << "DeleteSeanceDialog::DeleteSeance Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }

}

void DeleteSeanceDialog::FillSeanceComboBox() {
    try {
        ui_->seanceComboBox->clear();
        auto treatment = repo_->GetTreatment(dossier_);
        auto seances = treatment.GetAllSeances();

        for (int s_idx = 0; s_idx < (int)seances.size(); ++s_idx) {
            if (seances.at(s_idx).GetSeanceRecords().empty()) {
                ui_->seanceComboBox->addItem(QString::number(s_idx + 1) + ": " + QString::number(seances.at(s_idx).GetDosePrescribed(), 'f', 3) + " Gy",
                                             seances.at(s_idx).GetSeanceId());
            }
        }
    }
    catch (std::exception& exc) {
        qWarning() << "DeleteSeanceDialog::FillSeanceComboBox Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}



