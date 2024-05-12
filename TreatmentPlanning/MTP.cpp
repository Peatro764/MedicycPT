#include "MTP.h"
#include "ui_MTP.h"

#include <QDebug>
#include <QStandardPaths>
#include <QMessageBox>

#include "PrintDialog.h"
#include "ConfigDialog.h"

MTP::MTP(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::MTP),
    settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MTP.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat),
    dossier_page_(nullptr),
    library_page_(nullptr),
    collimateur_page_(nullptr),
    modulateur_page_(nullptr),
    math_util_page_(nullptr),
    pt_repo_(nullptr),
    qa_repo_(nullptr),
    login_dialog_(parent),
    logged_in_(true)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Medicyc Treatment Planning (MTP) v1.3.2"));
    SetupDb();
    SetupLineValidators();
    SetupDefaults();
    DisableProtectedSections(false);
    //GetUsers();

    library_page_ = new LibraryPage(this);
    dossier_page_ = new DossierPage(this);
    collimateur_page_ = new CollimateurPage(this);
    compensateur_page_ = new CompensateurPage(this);
    modulateur_page_ = new ModulateurPage(this);
    math_util_page_ = new MathUtilsPage(this);
    login_dialog_.setParent(this, Qt::Dialog);

    SetupConnections();
    GotoTopMenu();
    setContextMenuPolicy(Qt::CustomContextMenu);
}

MTP::~MTP()
{
    if (pt_repo_) delete pt_repo_;
    if (qa_repo_) delete qa_repo_;
    if (dossier_page_) delete dossier_page_;
    if (library_page_) delete library_page_;
    if (collimateur_page_) delete collimateur_page_;
    if (compensateur_page_) delete compensateur_page_;
    if (modulateur_page_) delete modulateur_page_;
    if (math_util_page_) delete math_util_page_;
}

void MTP::LaunchPrintDialog(int dossier) {
    if (!pt_repo_->CheckConnection()) { return; }
    if (!qa_repo_->CheckConnection()) { return; }
    PrintDialog dialog(this, pt_repo_, qa_repo_, dossier);
    dialog.exec();
}

void MTP::SetupConnections() {
    QObject::connect(ui_->stackedWidget , SIGNAL(currentChanged(int)), this, SLOT(UpdateTitle(int)));
    QObject::connect(ui_->configPushButton, SIGNAL(clicked()), this, SLOT(ShowConfigurationDialog()));
    QObject::connect(dossier_page_, &DossierPage::PrintDossier, this, &MTP::LaunchPrintDialog);
    QObject::connect(ui_->dbPushButton, SIGNAL(clicked()), this, SLOT(SetupDb()));
    QObject::connect(ui_->collimateurPushButton, SIGNAL(clicked()), this, SLOT(ShowCollimateurWidget()));
    QObject::connect(ui_->compensateurPushButton, SIGNAL(clicked()), this, SLOT(ShowCompensateurWidget()));
    QObject::connect(ui_->moddeglibPushButton, SIGNAL(clicked()), this, SLOT(ShowModDegLibWidget()));
    QObject::connect(ui_->dossierPushButton, SIGNAL(clicked()), this, SLOT(ShowDossierWidget()));
    QObject::connect(ui_->createModulateurPushButton, SIGNAL(clicked()), this, SLOT(ShowModulateurWidget()));
    QObject::connect(ui_->mathUtilsPushButton, SIGNAL(clicked()), this, SLOT(ShowMathUtilsWidget()));
    QObject::connect(ui_->topNavigPushButton, SIGNAL(clicked()), this, SLOT(GotoTopMenu()));
    QObject::connect(ui_->leftNavigPushButton, SIGNAL(clicked()), this, SLOT(GotoLeftMenu()));
    QObject::connect(ui_->rightNavigPushButton, SIGNAL(clicked()), this, SLOT(GotoRightMenu()));
    QObject::connect(ui_->quitPushButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(&login_dialog_, SIGNAL (acceptLogin(QString&,QString&,int&)), this, SLOT (AcceptUserLogin(QString&,QString&)));
    QObject::connect(ui_->loginButton, SIGNAL(clicked()), this, SLOT(LoginLogout()));
}

void MTP::GetUsers() {
    try {
    login_dialog_.setUsernamesList(pt_repo_->GetOperators());
    }
    catch (std::exception& exc) {
        qWarning() << "MTP::GetUsers Failed retreiving users: " << exc.what();
    }
}

void MTP::LoginLogout() {
    if (logged_in_) {
        DisableProtectedSections(true);
    } else {
        if (login_dialog_.exec() == QDialog::Accepted) {
            DisableProtectedSections(false);
        }
    }
}

void MTP::AcceptUserLogin(QString& userName, QString& password) {
    try {
        if (!pt_repo_->CheckConnection()) { return; }
        QString truePasswordHashed(pt_repo_->GetOperatorPassword(userName));
        QString givenPasswordHashed(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
        if (truePasswordHashed == givenPasswordHashed) {
            qDebug() << "MTP::AcceptUserLogin Given password for user " + userName + " is correct";
            login_dialog_.LoginOk();
        } else {
            qDebug() << "MTP::AcceptUserLogin Given password for user " + userName + " is incorrect";
            throw std::runtime_error("Wrong password");
        }
    }
    catch (std::exception& exc) {
        qWarning() << "MTP::AcceptUserLogin " << exc.what();
        QMessageBox::information(this, tr("Login error"), tr("Please try again"));
    }
}

void MTP::DisableProtectedSections(bool disable) {
    if (disable) {
        ui_->loginButton->setText("Login");
        logged_in_ = false;
    } else {
        ui_->loginButton->setText("Logout");
        logged_in_ = true;
    }
    ui_->dossierPushButton->setEnabled(!disable);
    ui_->moddeglibPushButton->setEnabled(!disable);
    ui_->createModulateurPushButton->setEnabled(!disable);
    ui_->collimateurPushButton->setEnabled(!disable);
    ui_->compensateurPushButton->setEnabled(!disable);
}

void MTP::UpdateTitle(int index) {
    switch (index) {
    case 0:
        ui_->pageLabel->setText("Menu");
        break;
    case 1:
        ui_->pageLabel->setText("Dossiers");
        dossier_page_->GetDossiers();
        break;
    case 2:
        ui_->pageLabel->setText("Libraire d'Accessoires");
        break;
    case 3:
        ui_->pageLabel->setText("Outils de calcul");
        break;
    case 4:
        ui_->pageLabel->setText("Collimateur");
        break;
    case 5:
        ui_->pageLabel->setText("Compensateur");
        break;
    case 6:
        ui_->pageLabel->setText("Modulateur");
        break;
    default:
        ui_->pageLabel->setText("");
        break;
    }
}


void MTP::ShowConfigurationDialog() {
    ConfigDialog configDialog;
    configDialog.show();
    configDialog.exec();
    configDialog.hide();
}

void MTP::GotoTopMenu() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::START);
}

void MTP::GotoLeftMenu() {
    if (!logged_in_) {
        return;
    }
    if (ui_->stackedWidget->currentIndex() == TabWidget::START) {
        return;
    }
    if (ui_->stackedWidget->currentIndex() > 1) {
        ui_->stackedWidget->setCurrentIndex(ui_->stackedWidget->currentIndex() - 1);
    } else {
        ui_->stackedWidget->setCurrentIndex(ui_->stackedWidget->count() - 1);
    }
}

void MTP::GotoRightMenu() {
    if (!logged_in_) {
        return;
    }
    if (ui_->stackedWidget->currentIndex() == TabWidget::START) {
        return;
    }
    if (ui_->stackedWidget->currentIndex() != (ui_->stackedWidget->count() - 1)) {
        ui_->stackedWidget->setCurrentIndex(ui_->stackedWidget->currentIndex() + 1);
    } else {
        ui_->stackedWidget->setCurrentIndex(1);
    }
}

void MTP::SetupLineValidators() {}

void MTP::SetupDefaults() {
    ui_->loginButton->setVisible(false);
    ui_->listDossier_dossierTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MTP::SetupDb() {
    if (pt_repo_) {
        delete pt_repo_;
        pt_repo_ = nullptr;
    }
    if (qa_repo_) {
        delete qa_repo_;
        qa_repo_ = nullptr;
    }
    TurnOffDbButton();

    QString pt_dbHostName = settings_.value("ptdatabase/host", "unknown").toString();
    QString pt_dbDatabaseName = settings_.value("ptdatabase/name", "unknown").toString();
    int pt_dbPort = settings_.value("ptdatabase/port", "5432").toInt();
    QString pt_dbUserName = settings_.value("ptdatabase/user", "unknown").toString();
    QString pt_dbPassword = settings_.value("ptdatabase/password", "unknown").toString();
    int pt_deconnect_on_idle_timeout = settings_.value("ptdatabase/disconnect_on_idle_timeout", "600").toInt();
    QString pt_dbConnName = "ptdb";

    if (pt_dbHostName == "unknown") qCritical() << "MTP::SetupDb Unknown db hostname";
    if (pt_dbDatabaseName == "unknown") qCritical() << "MTP::SetupDb Unknown db name";
    if (pt_dbUserName == "unknown") qCritical() << "MTP::SetupDb Unknown db username";
    if (pt_dbPassword == "unknown") qCritical() << "MTP::SetupDb Unknown db password";

    pt_repo_ = new PTRepo(pt_dbConnName, pt_dbHostName, pt_dbPort, pt_dbDatabaseName, pt_dbUserName, pt_dbPassword, pt_deconnect_on_idle_timeout);
    QObject::connect(pt_repo_, SIGNAL(SIGNAL_ConnectionClosed()), this, SLOT(TurnOffDbButton()));
    QObject::connect(pt_repo_, SIGNAL(SIGNAL_ConnectionOpened()), this, SLOT(TurnOnDbButton()));
    QObject::connect(pt_repo_, SIGNAL(SIGNAL_FailedOpeningConnection()), this, SLOT(FailedConnectingToDatabase()));
    QObject::connect(pt_repo_, SIGNAL(SIGNAL_FailedOpeningConnection()), this, SLOT(TurnOffDbButton()));
    pt_repo_->Connect();

    QString qa_dbHostName = settings_.value("qadatabase/host", "unknown").toString();
    QString qa_dbDatabaseName = settings_.value("qadatabase/name", "unknown").toString();
    int qa_dbPort = settings_.value("qadatabase/port", "5432").toInt();
    QString qa_dbUserName = settings_.value("qadatabase/user", "unknown").toString();
    QString qa_dbPassword = settings_.value("qadatabase/password", "unknown").toString();
    int qa_deconnect_on_idle_timeout = settings_.value("qadatabase/disconnect_on_idle_timeout", "600").toInt();
    QString qa_dbConnName = "qadb";

    if (qa_dbHostName == "unknown") qCritical() << "MTP::SetupDb Unknown db hostname";
    if (qa_dbDatabaseName == "unknown") qCritical() << "MTP::SetupDb Unknown db name";
    if (qa_dbUserName == "unknown") qCritical() << "MTP::SetupDb Unknown db username";
    if (qa_dbPassword == "unknown") qCritical() << "MTP::SetupDb Unknown db password";

    qa_repo_ = new QARepo(qa_dbConnName, qa_dbHostName, qa_dbPort, qa_dbDatabaseName, qa_dbUserName, qa_dbPassword, qa_deconnect_on_idle_timeout);
    QObject::connect(qa_repo_, SIGNAL(SIGNAL_ConnectionClosed()), this, SLOT(TurnOffDbButton()));
    QObject::connect(qa_repo_, SIGNAL(SIGNAL_ConnectionOpened()), this, SLOT(TurnOnDbButton()));
    QObject::connect(qa_repo_, SIGNAL(SIGNAL_FailedOpeningConnection()), this, SLOT(FailedConnectingToDatabase()));
    QObject::connect(qa_repo_, SIGNAL(SIGNAL_FailedOpeningConnection()), this, SLOT(TurnOffDbButton()));
    qa_repo_->Connect();
}

void MTP::ShowDossierWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::DOSSIER);
}

void MTP::ShowModDegLibWidget() {
     ui_->stackedWidget->setCurrentIndex(TabWidget::MODDEGLIB);
}

void MTP::ShowCollimateurWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::COLLIMATEUR);
}

void MTP::ShowCompensateurWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::COMPENSATEUR);
}

void MTP::ShowModulateurWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::MODULATEUR);
}

void MTP::ShowMathUtilsWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::MATHUTIL);
}

void MTP::TurnOffDbButton() {
    ui_->dbPushButton->setStyleSheet(ButtonStyleSheet("db_button_red.png"));
}

void MTP::TurnOnDbButton() {
    ui_->dbPushButton->setStyleSheet(ButtonStyleSheet("db_button_green.png"));
}

QString MTP::ButtonStyleSheet(QString image) {
    QString str ("QPushButton {"
                 "background-image: url(:/images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "border-style: raised; "
    "border-width: 0px;"
    "border-radius: 25px;"
    "};"
    "QPushButton:pressed {"
    "border-width: 4px."
    "};");
    return str;
}

void MTP::FailedConnectingToDatabase() {
    QMessageBox::critical(this, "MTP", "Echec de la connexion à la base de données");
}
