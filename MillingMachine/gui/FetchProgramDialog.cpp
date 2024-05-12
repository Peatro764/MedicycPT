#include "FetchProgramDialog.h"
#include "ui_FetchProgramDialog.h"

#include "Collimator.h"
#include "Compensateur.h"

FetchProgramDialog::FetchProgramDialog(QWidget *parent, PTRepo *repo) :
    QDialog(parent),
    ui_(new Ui::FetchProgramDialog),
    repo_(repo),
    id_(""),
    program_type_("")
{
    ui_->setupUi(this);
    setStyle(QStyleFactory::create("windowsxp"));
    setWindowTitle("Telechargement Programme Fraiseuse");

    ui_->treeWidget->setColumnCount(3);
    ui_->treeWidget->setHeaderLabels(QStringList() << "Type" << "Id" << "L'heure de creation");
    ui_->treeWidget->header()->resizeSection(0, 140);
    ui_->treeWidget->header()->resizeSection(1, 100);
    QTreeWidgetItem *misc = new QTreeWidgetItem((QTreeWidget*)0, QStringList() << "MISC" << "" << "");
    QList<QTreeWidgetItem*> top_level_items;
    top_level_items.append(misc);
    ui_->treeWidget->insertTopLevelItems(0, top_level_items);

    ConnectSignals();
    FillProgramTypes();
    FillMiscDirectories(misc);
    FillTreatmentDirectories();
}

FetchProgramDialog::~FetchProgramDialog() {
    delete ui_;
}

void FetchProgramDialog::ConnectSignals() {
//    QObject::connect(ui_->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(ItemClicked(QTreeWidgetItem*, int)));
    QObject::connect(ui_->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(DownloadItem(QTreeWidgetItem*, int)));
    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void FetchProgramDialog::ItemClicked(QTreeWidgetItem *item, int column) {
    (void)column;
    qDebug() << item->text(0) << " " << item->text(1) << " " << item->text(2);
}

void FetchProgramDialog::DownloadItem(QTreeWidgetItem *item, int column) {
    (void)column;
    try {
        qDebug() << item->text(0) << " " << item->text(1) << " " << item->text(2);
        QTreeWidgetItem *parent_item = item->parent();
        if (!parent_item) {
            qWarning() << "FetchProgramDialog::DownloadItem Neither a misc or a treatment program type";
            return;
        }

        QString type(parent_item->text(0));
        if (!misc_types_.contains(type) && !treatment_types_.contains(type)) {
            return;
        }

        program_type_ = parent_item->text(0);
        id_ = item->text(1);
        timestamp_ = QDateTime::fromString(item->text(2));

        if (misc_types_.contains(type)) {
             program_ = repo_->GetMiscFraiseuseProgram(id_, program_type_);
        } else {
            program_ = repo_->GetTreatmentFraiseuseProgram(id_.toInt(), program_type_);
        }

        accept();
    }
    catch (std::exception& exc) {
        qWarning() << "FetchProgramDialog::DownloadItem Exception thrown: " << exc.what();
    }
}

void FetchProgramDialog::FillProgramTypes() {
    try {
        treatment_types_ = repo_->GetFraiseuseTreatmentTypes();
        misc_types_ = repo_->GetFraiseuseMiscTypes();
    }
    catch (std::exception& exc) {
         qWarning() << "FetchProgramDialog::FillProgramTypes Exception thrown: " << exc.what();
    }
}

void FetchProgramDialog::FillTreatmentDirectories() {
    for (auto dir : treatment_types_) {
        QTreeWidgetItem *treat_dir = new QTreeWidgetItem(ui_->treeWidget);
        treat_dir->setText(0, dir);
        treat_dir->setText(1, "");
        treat_dir->setText(2, "");
        FillTreatmentData(treat_dir);
    }
}

void FetchProgramDialog::FillTreatmentData(QTreeWidgetItem * parent) {
    try {
        auto programs(repo_->GetFraiseuseTreatmentPrograms(parent->text(0)));
        for (auto it = programs.rbegin(); it != programs.rend(); ++it) {
            AddLeaf(parent, QString::number(it->second), it->first);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "FetchProgramDialog::FillCollimateurData Exception thrown: " << exc.what();
    }
}

void FetchProgramDialog::FillMiscDirectories(QTreeWidgetItem *parent) {
    for (auto dir : misc_types_) {
        QTreeWidgetItem *misc_dir = new QTreeWidgetItem();
        misc_dir->setText(0, dir);
        misc_dir->setText(1, "");
        misc_dir->setText(2, "");
        parent->addChild(misc_dir);
        FillMiscData(misc_dir);
    }
}

void FetchProgramDialog::FillMiscData(QTreeWidgetItem * item) {
    try {
        auto programs(repo_->GetFraiseuseMiscPrograms(item->text(0)));
        for (auto it = programs.rbegin(); it != programs.rend(); ++it) {
            AddLeaf(item, it->second, it->first);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "FetchProgramDialog::FillMiscData Exception thrown: " << exc.what();
    }
}

void FetchProgramDialog::AddLeaf(QTreeWidgetItem *parent, QString name, QDateTime timestamp) {
    QTreeWidgetItem *leaf = new QTreeWidgetItem();
    leaf->setText(0, "");
    leaf->setText(1, name);
    leaf->setText(2, timestamp.toString());
    parent->addChild(leaf);
}
