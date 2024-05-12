#include "MillingMachineProgramDialog.h"
#include "ui_MillingMachineProgramDialog.h"

#include "QFileDialog"
#include "QTextStream"
#include "QMessageBox"

MillingMachineProgramDialog::MillingMachineProgramDialog(QWidget *parent, QString program) :
    QDialog(parent),
    ui_(new Ui::MillingMachineProgramDialog),
    program_(program)
{
    ui_->setupUi(this);
    QObject::connect(ui_->pushButton_close, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui_->pushButton_saveToFile, SIGNAL(clicked()), this, SLOT(SaveToFile()));
    setWindowTitle("Fraiseuse Programme");
    ui_->programPlainTextEdit->setPlainText(program_);
}

void MillingMachineProgramDialog::SaveToFile() {
    QString filename(QFileDialog::getSaveFileName(this, "Sauvegarde dans un fichier",
                                                  "program.mil", "*.mil"));
    if (filename.isNull() || filename.isEmpty()) {
        //QMessageBox::warning(this, "MTP", "Nom de fichier ou répertoire corrompu");
        return;
    }

    QFile program_file(filename);
    if (!program_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "MTP", "Impossible d'ouvrir le fichier");
        return;
    }

    QTextStream stream(&program_file);
    stream << program_;
    program_file.close();
}



