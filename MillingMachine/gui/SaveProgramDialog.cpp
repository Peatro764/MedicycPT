#include "SaveProgramDialog.h"
#include "ui_SaveProgramDialog.h"

#include "Collimator.h"
#include "Compensateur.h"

SaveProgramDialog::SaveProgramDialog(QWidget *parent, PTRepo *repo, QString program_id, QString program_type) :
    QDialog(parent),
    ui_(new Ui::SaveProgramDialog),
    repo_(repo),
    program_id_(program_id),
    program_type_(program_type)
{
    ui_->setupUi(this);
    setWindowTitle("Sauvegarde Programme Fraiseuse");

    ui_->treeWidget->setColumnCount(1);
    ui_->treeWidget->setHeaderLabels(QStringList() << "Type");
    ui_->nameLineEdit->setText(program_id_);

    ConnectSignals();
    FillProgramTypes();
    FillMiscDirectories();
    SetCurrentDirectory(program_type_);
}

SaveProgramDialog::~SaveProgramDialog() {
    delete ui_;
}

void SaveProgramDialog::ConnectSignals() {
//    QObject::connect(ui_->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(ItemClicked(QTreeWidgetItem*, int)));
    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(ui_->savePushButton , SIGNAL(clicked()), this, SLOT(SaveProgram()));

}

void SaveProgramDialog::ItemClicked(QTreeWidgetItem *item, int column) {
    (void)column;
    qDebug() << item->text(0) << " " << item->text(1) << " " << item->text(2);
}

void SaveProgramDialog::FillProgramTypes() {
    try {
        misc_types_ = repo_->GetFraiseuseMiscTypes();
    }
    catch (std::exception& exc) {
         qWarning() << "SaveProgramDialog::FillProgramTypes Exception thrown: " << exc.what();
    }
}

void SaveProgramDialog::FillMiscDirectories() {
    for (auto dir : misc_types_) {
        QTreeWidgetItem *misc_dir = new QTreeWidgetItem(ui_->treeWidget);
        misc_dir->setText(0, dir);
        misc_dir->setText(1, "");
        misc_dir->setText(2, "");
    }
}

void SaveProgramDialog::SaveProgram() {
    QString program_id(ui_->nameLineEdit->text());
    if (program_id.isEmpty() || program_id.isNull()) {
        QMessageBox::warning(this, "Fraiseuse", "Please give a program name");
        return;
    }

    QString program_type(ui_->treeWidget->currentItem()->text(0));
    if (program_type.isNull() || program_type.isEmpty()) {
        QMessageBox::warning(this, "Fraiseuse", "Please select a program type");
        return;
    }

    program_id_ = program_id;
    program_type_ = program_type;
    accept();
}

void SaveProgramDialog::SetCurrentDirectory(QString program_type) {
    QList<QTreeWidgetItem*> dirs(ui_->treeWidget->findItems(program_type, Qt::MatchExactly));
    if (dirs.size() == 1) {
        ui_->treeWidget->setCurrentItem(dirs.at(0));
    }
}
