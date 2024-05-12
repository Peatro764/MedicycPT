#include "FetchSOBPDialog.h"
#include "ui_FetchSOBPDialog.h"

FetchSOBPDialog::FetchSOBPDialog(QWidget *parent, std::shared_ptr<QARepo> repo) :
    QDialog(parent),
    ui_(new Ui::FetchSOBPDialog),
    repo_(repo),
    sobp_(DepthDoseMeasurement(QA_HARDWARE::UNK), 0, 0, 0.0, "Empty", 0.0, 0.0, 0.0, 0.0)
{
    ui_->setupUi(this);
    setStyle(QStyleFactory::create("windowsxp"));
    setWindowTitle("SOBPs");

    ui_->treeWidget->setColumnCount(5);
    ui_->treeWidget->setHeaderLabels(QStringList() << "Modulateur" << "Degradeur\n[mm plexi]" << "Dossier" << "Commentaire" << "L'heure de creation");
    ui_->treeWidget->header()->resizeSection(0, 100);
    ui_->treeWidget->header()->resizeSection(1, 85);
    ui_->treeWidget->header()->resizeSection(2, 70);

    ConnectSignals();
    FillDirectories();
}

FetchSOBPDialog::~FetchSOBPDialog() {
    delete ui_;
}

void FetchSOBPDialog::ConnectSignals() {
    QObject::connect(ui_->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(DownloadItem(QTreeWidgetItem*, int)));
    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void FetchSOBPDialog::ItemClicked(QTreeWidgetItem *item, int column) {
    (void)column;
    qDebug() << item->text(0) << " " << item->text(1) << " " << item->text(2);
}

void FetchSOBPDialog::DownloadItem(QTreeWidgetItem *item, int column) {
    (void)column;
    try {
        qDebug() << item->text(0) << " " << item->text(1) << " " << item->text(2);
        QTreeWidgetItem *parent_item = item->parent();
        if (!parent_item) {
            qWarning() << "FetchSOBPDialog::DownloadItem Didnt find the item";
            return;
        }

        QDateTime timestamp = QDateTime::fromString(item->text(4), "yyyy-MM-dd hh:mm:ss.zzz");
        qDebug() << "FetchSOBPDialog::DownloadItem Getting sobp with timestamp: " << timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");
        sobp_ = repo_->GetSOBP(timestamp);
        accept();
    }
    catch (std::exception& exc) {
        qWarning() << "FetchSOBPDialog::DownloadItem Exception thrown: " << exc.what();
        DisplayModelessMessageBox(exc.what(), QMessageBox::Critical);
    }
}

void FetchSOBPDialog::FillDirectories() {
    try {
        auto modulateurs = repo_->GetSOBPModulateurs();
        for (auto m : modulateurs) {
            QTreeWidgetItem *dir = new QTreeWidgetItem(ui_->treeWidget);
            dir->setText(0, QString::number(m));
            dir->setText(1, "");
            dir->setText(2, "");
            dir->setText(3, "");
            FillData(dir);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "FetchSOBPDialog::DownloadItem Exception thrown: " << exc.what();
        DisplayModelessMessageBox(exc.what(), QMessageBox::Critical);
    }
}

void FetchSOBPDialog::FillData(QTreeWidgetItem * parent) {
    try {
        std::map<QDateTime, ModAcc> sobps(repo_->GetSOBPSForModulateur(parent->text(0).toInt()));
        for (auto const& s : sobps) {
            AddLeaf(parent, s.first, s.second.rs_mm_plexi(), s.second.dossier(), s.second.comment());
        }
        parent->sortChildren(1, Qt::AscendingOrder);
    }
    catch (std::exception& exc) {
        qWarning() << "FetchSOBPDialog::FillCollimateurData Exception thrown: " << exc.what();
        DisplayModelessMessageBox(exc.what(), QMessageBox::Critical);
    }
}

void FetchSOBPDialog::AddLeaf(QTreeWidgetItem *parent, QDateTime timestamp, double rs_mm_plexi, int dossier, QString comment) {
    QTreeWidgetItem *leaf = new QTreeWidgetItem();
    leaf->setText(0, "");
    leaf->setText(1, QString::number(rs_mm_plexi, 'f', 1));
    QString dossierText = dossier == 0 ? QString("-") : QString::number(dossier);
    leaf->setText(2, dossierText);
    leaf->setText(3, comment);
    leaf->setText(4, timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz"));
    parent->addChild(leaf);
}

void FetchSOBPDialog::DisplayModelessMessageBox(QString msg, QMessageBox::Icon icon) {
    QMessageBox* box = new QMessageBox(this);
    box->setText(msg);
    box->setIcon(icon);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}
