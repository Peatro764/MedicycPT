#include "CommentsDialog.h"
#include "ui_CommentsDialog.h"


CommentsDialog::CommentsDialog(QWidget *parent,PTRepo* repo, int dossier) :
    QDialog(parent),
    ui_(new Ui::CommentsDialog),
    repo_(repo),
    dossier_(dossier)
{
    ui_->setupUi(this);
    setWindowTitle("Commentaires");

    Fill();

    QObject::connect(ui_->pushButton_quit, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui_->pushButton_save, SIGNAL(clicked()), this, SLOT(Save()));
}

void CommentsDialog::Save() {
    try {
        repo_->SetDossierComments(dossier_, ui_->plainTextEdit->toPlainText());
        QMessageBox::information(this, "MTP", "Comments saved");
    }
    catch (std::exception& exc) {
        qWarning() << "CommentsDialog::Fill Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}

void CommentsDialog::Fill() {
    try {
        ui_->plainTextEdit->setPlainText(repo_->GetDossierComments(dossier_));
        ui_->label_dossier->setText(QString::number(dossier_));
    }
    catch (std::exception& exc) {
        qWarning() << "CommentsDialog::Fill Caught exception: " << exc.what();
        QMessageBox::warning(this, "MTP", QString("An exception occurred: ") + exc.what());
    }
}
