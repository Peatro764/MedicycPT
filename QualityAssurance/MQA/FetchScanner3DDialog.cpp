#include "FetchScanner3DDialog.h"
#include "ui_FetchScanner3DDialog.h"

FetchScanner3DDialog::FetchScanner3DDialog(QWidget *parent, std::shared_ptr<QARepo> repo) :
    QDialog(parent),
    ui_(new Ui::FetchScanner3DDialog),
    repo_(repo)
{
    ui_->setupUi(this);
    setStyle(QStyleFactory::create("windowsxp"));
    setWindowTitle("Telechargement 3D cubes");

    ui_->treeWidget->setColumnCount(2);
    ui_->treeWidget->setHeaderLabels(QStringList() << "L'heure de creation" << "Commentaire");
    ui_->treeWidget->header()->resizeSection(0, 250);
    ui_->treeWidget->header()->resizeSection(1, 100);

    ConnectSignals();
    FillDirectories();
}

FetchScanner3DDialog::~FetchScanner3DDialog() {
    delete ui_;
}

void FetchScanner3DDialog::ConnectSignals() {
    QObject::connect(ui_->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(DownloadItem(QTreeWidgetItem*, int)));
    QObject::connect(ui_->cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void FetchScanner3DDialog::ItemClicked(QTreeWidgetItem *item, int column) {
    (void)column;
    qDebug() << item->text(0) << " " << item->text(1);
}

void FetchScanner3DDialog::DownloadItem(QTreeWidgetItem *item, int column) {
    (void)column;
    try {
        qDebug() << item->text(0) << " " << item->text(1);
//        QTreeWidgetItem *parent_item = item->parent();
//        if (!parent_item) {
//            qWarning() << "FetchScanner3DDialog::DownloadItem Didnt find the item";
//            return;
//        }

        QDateTime timestamp = QDateTime::fromString(item->text(0), "yyyy-MM-dd hh:mm:ss.zzz");
        qDebug() << "FetchScanner3DDialog::DownloadItem Getting sobp with timestamp: " << timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");
        cube_ = repo_->GetCube(timestamp);
        accept();
    }
    catch (std::exception& exc) {
        qWarning() << "FetchScanner3DDialog::DownloadItem Exception thrown: " << exc.what();
        DisplayModelessMessageBox(exc.what(), QMessageBox::Critical);
    }
}

void FetchScanner3DDialog::FillDirectories() {
    try {
        auto keys = repo_->GetCubeKeys();
        for (auto k : keys) {
            QTreeWidgetItem *dir = new QTreeWidgetItem(ui_->treeWidget);
            dir->setText(0, k.first.toString("yyyy-MM-dd hh:mm:ss.zzz"));
            dir->setText(1, k.second);
//            FillData(dir);
        }
    }
    catch (std::exception& exc) {
        qWarning() << "FetchScanner3DDialog::DownloadItem Exception thrown: " << exc.what();
        DisplayModelessMessageBox(exc.what(), QMessageBox::Critical);
    }
}

void FetchScanner3DDialog::FillData(QTreeWidgetItem * parent) {
    (void)parent;
//    try {
//        auto sobps(repo_->GetSOBPSForModulateur(parent->text(0).toInt()));
//        for (auto const& s : sobps) {
//            AddLeaf(parent, s.first, s.second);
//        }
//    }
//    catch (std::exception& exc) {
//        qWarning() << "FetchScanner3DDialog::FillCollimateurData Exception thrown: " << exc.what();
//        DisplayModelessMessageBox(exc.what(), QMessageBox::Critical);
//    }
}

void FetchScanner3DDialog::AddLeaf(QTreeWidgetItem *parent, QDateTime timestamp, QString comment) {
    QTreeWidgetItem *leaf = new QTreeWidgetItem();
    leaf->setText(0, "");
    leaf->setText(1, timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz"));
    leaf->setText(2, comment);
    parent->addChild(leaf);
}

void FetchScanner3DDialog::DisplayModelessMessageBox(QString msg, QMessageBox::Icon icon) {
    QMessageBox* box = new QMessageBox(this);
    box->setText(msg);
    box->setIcon(icon);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}
