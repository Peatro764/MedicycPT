#include "SOBPResultsDialog.h"
#include "ui_SOBPResultsDialog.h"

#include "Material.h"

SOBPResultsDialog::SOBPResultsDialog(QWidget *parent, const SOBP& sobp) :
    QDialog(parent),
    ui_(new Ui::SOBPResultsDialog),
    sobp_(sobp)
{
    ui_->setupUi(this);
    setWindowTitle("SOBP Results");
    QObject::connect(ui_->pushButton_ok, SIGNAL(clicked()), this, SLOT(accept()));
    Fill();
}

void SOBPResultsDialog::Fill() {
    try {
        QString name =  QString(QString::number(sobp_.dossier())) + " " + QString::number(sobp_.modulateur_id()) + " " + QString::number(sobp_.mm_degradeur(), 'f', 1);
        ui_->lineEdit_name->setText(name);

        ui_->lineEdit_mod98_tissue->setText(QString::number(sobp_.mod98(), 'f', 2));
        ui_->lineEdit_mod100_tissue->setText(QString::number(sobp_.mod100(), 'f', 2));
        ui_->lineEdit_parcours_tissue->setText(QString::number(sobp_.parcours(), 'f', 2));
        ui_->lineEdit_penumbra_tissue->setText(QString::number(sobp_.penumbra(), 'f', 2));

        ui_->lineEdit_mod98_plexi->setText(QString::number(material::ToPlexiglas(sobp_.mod98(), material::MATERIAL::TISSUE) , 'f', 2));
        ui_->lineEdit_mod100_plexi->setText(QString::number(material::ToPlexiglas(sobp_.mod100(), material::MATERIAL::TISSUE), 'f', 2));
        ui_->lineEdit_parcours_plexi->setText(QString::number(material::ToPlexiglas(sobp_.parcours(), material::MATERIAL::TISSUE), 'f', 2));
        ui_->lineEdit_penumbra_plexi->setText(QString::number(material::ToPlexiglas(sobp_.penumbra(), material::MATERIAL::TISSUE), 'f', 2));
    }
    catch (std::exception& exc) {
        qWarning() << "SOBPResultsDialog::Fill Exception caught: " << exc.what();
    }
}
