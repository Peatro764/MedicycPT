#include "MQA.h"
#include "ui_MQA.h"

#include <QDebug>
#include <QStandardPaths>
#include <QMessageBox>

#include "ConfigDialog.h"
#include "PrintDialog.h"
#include "CSVDialog.h"
#include "ProfilePrintDialog.h"

MQA::MQA(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::MQA),
    settings_(std::shared_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MQA.ini"), QStandardPaths::LocateFile),
                                                          QSettings::IniFormat))),
    logged_in_(false)
{
    ui_->setupUi(this);
    setWindowTitle(QString("Medicyc Quality Assurance (MQA) v1.4.0"));
    SetupDb();
    SetupLineValidators();
    SetupPushButtonGraphics();
    SetupDefaults();

    beam_profiler_gui_ = std::unique_ptr<BeamProfilerGui>(new BeamProfilerGui(this, qa_repo_, QA_HARDWARE::SCANNER2D));
    bragg_peak_gui_ = std::unique_ptr<BraggPeakGui>(new BraggPeakGui(this, qa_repo_, QA_HARDWARE::WHEEL));
    scanner3D_gui_ = std::unique_ptr<Scanner3DGui>(new Scanner3DGui(this, qa_repo_));
    tdj_gui_ = std::unique_ptr<TopDeJourGui>(new TopDeJourGui(this, pt_repo_, qa_repo_));
    patient_debit_gui_ = std::unique_ptr<PatientDebitGui>(new PatientDebitGui(this, pt_repo_, qa_repo_));
    beamline_gui_ = std::unique_ptr<BeamLineGui>(new BeamLineGui(this, pt_repo_));

    SetupConnections();
    GotoTopMenu();
    setContextMenuPolicy(Qt::CustomContextMenu);
}

MQA::~MQA()
{
}

void MQA::SetupPushButtonGraphics() {
    ui_->beamProfilerPushButton->setIcon(QIcon(":/images/Profile.png"));
    ui_->beamProfilerPushButton->setIconSize(QSize(192,119));
    ui_->braggPeakPushButton->setIcon(QIcon(":/images/DepthDose.png"));
    ui_->braggPeakPushButton->setIconSize(QSize(120,120));
    ui_->topDeJourPushButton->setIcon(QIcon(":/images/doserate_tdj2.png"));
    ui_->topDeJourPushButton->setIconSize(QSize(180,180));
    ui_->patientDebitPushButton->setIcon(QIcon(":/images/doserate_patient2.png"));
    ui_->patientDebitPushButton->setIconSize(QSize(180,180));
    ui_->beamlinePushButton->setIcon(QIcon(":/images/beamline.png"));
    ui_->beamlinePushButton->setIconSize(QSize(140,140));
}

void MQA::SetupConnections() {
    QObject::connect(ui_->stackedWidget , SIGNAL(currentChanged(int)), this, SLOT(ConfigurePage(int)));
    QObject::connect(ui_->stackedWidget , SIGNAL(currentChanged(int)), this, SLOT(UpdateTitle(int)));
    QObject::connect(ui_->dbPushButton, SIGNAL(clicked()), this, SLOT(SetupDb()));
    QObject::connect(ui_->beamProfilerPushButton, SIGNAL(clicked()), this, SLOT(ShowBeamProfilerWidget()));
    QObject::connect(ui_->braggPeakPushButton, SIGNAL(clicked()), this, SLOT(ShowBraggPeakWidget()));
    QObject::connect(ui_->scanner3DPushButton, SIGNAL(clicked()), this, SLOT(ShowScanner3DWidget()));
    QObject::connect(ui_->topDeJourPushButton, SIGNAL(clicked()), this, SLOT(ShowTopDeJourWidget()));
    QObject::connect(ui_->patientDebitPushButton, SIGNAL(clicked()), this, SLOT(ShowPatientDebitWidget()));
    QObject::connect(ui_->beamlinePushButton, SIGNAL(clicked()), this, SLOT(ShowBeamLineWidget()));
    QObject::connect(ui_->topNavigPushButton, SIGNAL(clicked()), this, SLOT(GotoTopMenu()));
    QObject::connect(ui_->leftNavigPushButton, SIGNAL(clicked()), this, SLOT(GotoLeftMenu()));
    QObject::connect(ui_->rightNavigPushButton, SIGNAL(clicked()), this, SLOT(GotoRightMenu()));
    QObject::connect(ui_->quitPushButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui_->pushButton_print, SIGNAL(clicked()), this, SLOT(LaunchPrintDialog()));
    QObject::connect(ui_->pushButton_csv , SIGNAL(clicked()), this, SLOT(LaunchCSVDialog()));

    QObject::connect(ui_->p_radioButton_cuve, &QRadioButton::toggled, this, &MQA::BeamProfileHardwareChoiceChanged);
    QObject::connect(ui_->b_radioButton_cuve, &QRadioButton::toggled, this, &MQA::BraggPeakHardwareChoiceChanged);
}

void MQA::BeamProfileHardwareChoiceChanged(bool cuve_enabled) {
    qDebug() << "MQA::BeamProfileHardwareChoiceChanged " << cuve_enabled;
    beam_profiler_gui_->CleanUp();
    beam_profiler_gui_.reset();
    QA_HARDWARE hardware = (cuve_enabled ? QA_HARDWARE::SCANNER3D : QA_HARDWARE::SCANNER2D);
    beam_profiler_gui_ = std::unique_ptr<BeamProfilerGui>(new BeamProfilerGui(this, qa_repo_, hardware));
    DisplayModelessMessageBox("Reconfiguration of hardware in progress...", true, 3, QMessageBox::Information);
    beam_profiler_gui_->Configure();
}

void MQA::BraggPeakHardwareChoiceChanged(bool cuve_enabled) {
    bragg_peak_gui_->CleanUp();
    bragg_peak_gui_.reset();
    QA_HARDWARE hardware = (cuve_enabled ? QA_HARDWARE::SCANNER3D : QA_HARDWARE::WHEEL);
    bragg_peak_gui_ = std::unique_ptr<BraggPeakGui>(new BraggPeakGui(this, qa_repo_, hardware));
    DisplayModelessMessageBox("Reconfiguration of hardware in progress...", true, 3, QMessageBox::Information);
    bragg_peak_gui_->Configure();
}

void MQA::ConfigurePage(int index) {
    qDebug() << "MQA::ConfigurePage " << index;
    beam_profiler_gui_->CleanUp();
    bragg_peak_gui_->CleanUp();
    patient_debit_gui_->CleanUp();
    scanner3D_gui_->CleanUp();
    tdj_gui_->CleanUp();
    beamline_gui_->CleanUp();

    switch (index) {
    case TabWidget::START:
        break;
    case TabWidget::BEAMPROFILE:
        beam_profiler_gui_->Configure();
        break;
    case TabWidget::BRAGGPEAK:
        bragg_peak_gui_->Configure();
        break;
    case TabWidget::SCANNER3D:
        scanner3D_gui_->Configure();
        break;
    case TabWidget::TOPDEJOUR:
        tdj_gui_->Configure();
        break;
    case TabWidget::PATIENTDEBIT:
        patient_debit_gui_->Configure();
        break;
    case TabWidget::BEAMLINE:
        beamline_gui_->Configure();
        break;
    default:
        break;
    }
}

void MQA::UpdateTitle(int index) {
    switch (index) {
    case TabWidget::START:
        ui_->pageLabel->setText("Menu");
        break;
    case TabWidget::BEAMPROFILE:
        ui_->pageLabel->setText("Profils de faisceau");
         break;
    case TabWidget::BRAGGPEAK:
        ui_->pageLabel->setText("Rendement en profondeur");
        break;
    case TabWidget::SCANNER3D:
        ui_->pageLabel->setText("3D Scanner");
        break;
    case TabWidget::TOPDEJOUR:
        ui_->pageLabel->setText("Top du Jour");
        break;
    case TabWidget::PATIENTDEBIT:
        ui_->pageLabel->setText("Débit Patient");
        break;
    case TabWidget::BEAMLINE:
        ui_->pageLabel->setText("Voie de Faisceau");
        break;
    default:
        ui_->pageLabel->setText("");
        break;
    }
}

void MQA::GotoTopMenu() {
    switch (ui_->stackedWidget->currentIndex()) {
    case TabWidget::BEAMPROFILE:
        if (!beam_profiler_gui_->IsIdle()) {
            DisplayModelessMessageBox("Please wait until all axes have stopped before leaving the page", true, 5, QMessageBox::Information);
            return;
        }
        break;
    case TabWidget::BRAGGPEAK:
        if (!bragg_peak_gui_->IsIdle()) {
            DisplayModelessMessageBox("Please wait until all axes have stopped before leaving the page", true, 5, QMessageBox::Information);
            return;
        }
        break;
    default:
        break;
    }

    ui_->stackedWidget->setCurrentIndex(TabWidget::START);
}

void MQA::GotoLeftMenu() {
    switch (ui_->stackedWidget->currentIndex()) {
    case TabWidget::START:
        return;
        break;
    case TabWidget::BEAMPROFILE:
        if (!beam_profiler_gui_->IsIdle()) {
            DisplayModelessMessageBox("Please wait until all axes have stopped before leaving the page", true, 5, QMessageBox::Information);
            return;
        }
        break;
    case TabWidget::BRAGGPEAK:
        if (!bragg_peak_gui_->IsIdle()) {
            DisplayModelessMessageBox("Please wait until all axes have stopped before leaving the page", true, 5, QMessageBox::Information);
            return;
        }
        break;
    default:
        break;
    }


    if (ui_->stackedWidget->currentIndex() > 1) {
        ui_->stackedWidget->setCurrentIndex(ui_->stackedWidget->currentIndex() - 1);
    } else {
        ui_->stackedWidget->setCurrentIndex(ui_->stackedWidget->count() - 1);
    }
}

void MQA::GotoRightMenu() {
    switch (ui_->stackedWidget->currentIndex()) {
    case TabWidget::START:
        return;
        break;
    case TabWidget::BEAMPROFILE:
        if (!beam_profiler_gui_->IsIdle()) {
            DisplayModelessMessageBox("Please wait until all axes have stopped before leaving the page", true, 5, QMessageBox::Information);
            return;
        }
        break;
    case TabWidget::BRAGGPEAK:
        if (!bragg_peak_gui_->IsIdle()) {
            DisplayModelessMessageBox("Please wait until all axes have stopped before leaving the page", true, 5, QMessageBox::Information);
            return;
        }
        break;
    default:
        break;
    }

    if (ui_->stackedWidget->currentIndex() != (ui_->stackedWidget->count() - 1)) {
        ui_->stackedWidget->setCurrentIndex(ui_->stackedWidget->currentIndex() + 1);
    } else {
        ui_->stackedWidget->setCurrentIndex(1);
    }
}

void MQA::SetupLineValidators() {}

void MQA::SetupDefaults() {
}

void MQA::SetupDb() {
    TurnOffDbButton();
    QString qaHostName = settings_->value("qadatabase/host", "unknown").toString();
    QString qaDatabaseName = settings_->value("qadatabase/name", "unknown").toString();
    int qaPort = settings_->value("qadatabase/port", "0").toInt();
    QString qaUserName = settings_->value("qadatabase/user", "unknown").toString();
    QString qaPassword = settings_->value("qadatabase/password", "unknown").toString();
    QString qaConnName = "qadb";

    if (qaHostName == "unknown") qCritical() << "MQA::SetupDb Unknown db hostname";
    if (qaDatabaseName == "unknown") qCritical() << "MQA::SetupDb Unknown db name";
    if (qaPort == 0) qCritical() << "MQA::SetupDb Unknown db port";
    if (qaUserName == "unknown") qCritical() << "MQA::SetupDb Unknown db username";
    if (qaPassword == "unknown") qCritical() << "MQA::SetupDb Unknown db password";

    if (!qa_repo_) {
        qa_repo_ = std::shared_ptr<QARepo>(new QARepo(qaConnName, qaHostName, qaPort, qaDatabaseName, qaUserName, qaPassword));
    }

    QString ptHostName = settings_->value("ptdatabase/host", "unknown").toString();
    QString ptDatabaseName = settings_->value("ptdatabase/name", "unknown").toString();
    int ptPort = settings_->value("ptdatabase/port", "unknown").toInt();
    QString ptUserName = settings_->value("ptdatabase/user", "unknown").toString();
    QString ptPassword = settings_->value("ptdatabase/password", "unknown").toString();
    QString ptConnName = "ptdb";

    if (ptHostName == "unknown") qCritical() << "MQA::SetupDb Unknown db hostname";
    if (ptDatabaseName == "unknown") qCritical() << "MQA::SetupDb Unknown db name";
    if (ptUserName == "unknown") qCritical() << "MQA::SetupDb Unknown db username";
    if (ptPassword == "unknown") qCritical() << "MQA::SetupDb Unknown db password";

    if (!pt_repo_) {
        pt_repo_ = std::shared_ptr<PTRepo>(new PTRepo(ptConnName, ptHostName, ptPort, ptDatabaseName, ptUserName, ptPassword));
    }

    if (qa_repo_->Connect() && pt_repo_->Connect()) {
        qDebug() << "MQA::SetupDb Connection established";
        TurnOnDbButton();
    } else {
        qWarning() << "MQA::SetupDb Connection failed";
        TurnOffDbButton();
    }
}

void MQA::ShowBeamProfilerWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::BEAMPROFILE);
}

void MQA::ShowBraggPeakWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::BRAGGPEAK);
}

void MQA::ShowScanner3DWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::SCANNER3D);
}

void MQA::ShowTopDeJourWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::TOPDEJOUR);
}

void MQA::ShowPatientDebitWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::PATIENTDEBIT);
}

void MQA::ShowBeamLineWidget() {
    ui_->stackedWidget->setCurrentIndex(TabWidget::BEAMLINE);
}

void MQA::TurnOffDbButton() {
    ui_->dbPushButton->setStyleSheet(ButtonStyleSheet("db_button_red.png"));
}

void MQA::TurnOnDbButton() {
    ui_->dbPushButton->setStyleSheet(ButtonStyleSheet("db_button_green.png"));
}

QString MQA::ButtonStyleSheet(QString image) {
    QString str ("QPushButton {"
                 "background-image: url(:/images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "border-style: raised; "
    "border-width: 0px;"
    "border-radius: 25px;"
    "}"
    "QPushButton:pressed {"
    "border-style: 4px;"
    "border-width: 4px;"
    "border-color: white;"
    "}");
    return str;
}

void MQA::LaunchPrintDialog() {
    try {
    PrintDialog dialog(this, qa_repo_, pt_repo_);
    dialog.exec();
    }
    catch (std::exception& exc) {
        DisplayModelessMessageBox(QString::fromStdString(exc.what()), false, 0, QMessageBox::Information);
    }
}


void MQA::LaunchCSVDialog() {
    try {
        CSVDialog dialog(this, qa_repo_);
        dialog.exec();
    }
    catch (std::exception& exc) {
        DisplayModelessMessageBox(QString::fromStdString(exc.what()), false, 0, QMessageBox::Information);
    }
}

void MQA::DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon) {
    qDebug() << "MQA::DisplayModelessMessageBox " << msg;
    QMessageBox* box = new QMessageBox(this);
    box->setText(msg);
    box->setIcon(icon);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    if (auto_close) QTimer::singleShot(sec * 1000, box, SLOT(close()));
    box->show();
}


