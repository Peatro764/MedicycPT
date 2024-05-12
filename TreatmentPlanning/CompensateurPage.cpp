#include "CompensateurPage.h"

#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <unistd.h>

#include "ui_MTP.h"
#include "CompensateurTransparantDialog.h"
#include "MTP.h"
#include "Calc.h"
#include "Material.h"
#include "MillingMachine.h"
#include "MillingMachineProgramDialog.h"
#include "Modulateur.h"
#include "Degradeur.h"


CompensateurPage::CompensateurPage(MTP* parent)
    : parent_(parent),
      settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MTP.ini"), QStandardPaths::LocateFile),
                QSettings::IniFormat),
      compXY_graph_(parent->ui()->comp_compXYCustomplot),
      compXZ_graph_(parent->ui()->comp_compXZCustomplot),
      eyeXZ_graph_(parent->ui()->comp_eyeXZCustomplot),
      compError_graph_(parent->ui()->comp_errorHistogramCustomplot)
{
    SetupLineEditValidators();
    SetupDefaults();
//    SetGraphicsEffects();
    ConnectSignals();
}

CompensateurPage::~CompensateurPage()
{
}

void CompensateurPage::SetupLineEditValidators() {
    parent_->ui()->comp_dossierLineEdit->setValidator(new QRegExpValidator(QRegExp("\\d{1,6}"), parent_->ui()->comp_dossierLineEdit));
    parent_->ui()->comp_bolusThicknessLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->comp_bolusThicknessLineEdit));
    parent_->ui()->comp_eyeDiameterLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->comp_eyeDiameterLineEdit));
    parent_->ui()->comp_eyeDSclereLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->comp_eyeDSclereLineEdit));
    parent_->ui()->comp_parcoursLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->comp_parcoursLineEdit));
    parent_->ui()->comp_xcentreLineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->comp_xcentreLineEdit));
    parent_->ui()->comp_ycentreLineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->comp_ycentreLineEdit));
}

void CompensateurPage::ConnectSignals() {
    QObject::connect(parent_->ui()->comp_createCompensateurPushButton, SIGNAL(clicked()), this, SLOT(CreateAndDraw()));
    QObject::connect(parent_->ui()->comp_maxParcoursErrorDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(CreateAndDraw()));
    QObject::connect(parent_->ui()->comp_errorScalingHorizontalSlider, SIGNAL(valueChanged(int)) , this, SLOT(CreateAndDraw()));
    QObject::connect(parent_->ui()->comp_saveToDatabasePushButton, SIGNAL(clicked()), this, SLOT(SaveToDatabase()));
    QObject::connect(parent_->ui()->comp_fetchFromDatabasePushButton, SIGNAL(clicked()), this, SLOT(FetchFromDatabase()));
    QObject::connect(parent_->ui()->comp_showFraiseuseProgramPushButton, SIGNAL(clicked()), this, SLOT(ShowFraiseuseProgram()));
    QObject::connect(parent_->ui()->comp_printTransparancyPushButton, SIGNAL(clicked()), this, SLOT(PrintTransparancy()));
    QObject::connect(parent_->ui()->comp_loadDossierPushButton, SIGNAL(clicked()), this, SLOT(LoadDossier()));
}

void CompensateurPage::SetupDefaults() {
    parent_->ui()->comp_xcentreLineEdit->setText("0.0");
    parent_->ui()->comp_ycentreLineEdit->setText("0.0");

    parent_->ui()->comp_maxParcoursErrorDoubleSpinBox->setRange(0.1,10.0);
    parent_->ui()->comp_maxParcoursErrorDoubleSpinBox->setSingleStep(0.1);
    parent_->ui()->comp_maxParcoursErrorDoubleSpinBox->setValue(0.4);

    parent_->ui()->comp_errorScalingHorizontalSlider->setMinimum(1);
    parent_->ui()->comp_errorScalingHorizontalSlider->setMaximum(100);
    parent_->ui()->comp_errorScalingHorizontalSlider->setSingleStep(1);
    parent_->ui()->comp_errorScalingHorizontalSlider->setValue(100);

    parent_->ui()->comp_dossierLineEdit->setFocus();
}

void CompensateurPage::ClearParameters() {
    parent_->ui()->comp_bolusThicknessLineEdit->clear();
    parent_->ui()->comp_eyeDiameterLineEdit->clear();
    parent_->ui()->comp_eyeDSclereLineEdit->clear();
    parent_->ui()->comp_parcoursLineEdit->clear();
    parent_->ui()->comp_xcentreLineEdit->setText("0.0");
    parent_->ui()->comp_ycentreLineEdit->setText("0.0");

    parent_->ui()->comp_maxParcoursErrorDoubleSpinBox->setRange(0.1,10.0);
    parent_->ui()->comp_maxParcoursErrorDoubleSpinBox->setSingleStep(0.1);
    parent_->ui()->comp_maxParcoursErrorDoubleSpinBox->setValue(0.4);
}

void CompensateurPage::SetGraphicsEffects() {
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setBlurRadius(2);
    parent_->ui()->comp_compXYCustomplot->setGraphicsEffect(effect);
    QGraphicsDropShadowEffect* effect2 = new QGraphicsDropShadowEffect();
    effect2->setBlurRadius(2);
    parent_->ui()->comp_compXZCustomplot->setGraphicsEffect(effect2);
    QGraphicsDropShadowEffect* effect3 = new QGraphicsDropShadowEffect();
    effect3->setBlurRadius(2);
    parent_->ui()->comp_compXZCustomplot->setGraphicsEffect(effect3);
    QGraphicsDropShadowEffect* effect4 = new QGraphicsDropShadowEffect();
    effect4->setBlurRadius(2);
    parent_->ui()->comp_eyeXZCustomplot->setGraphicsEffect(effect4);
    QGraphicsDropShadowEffect* effect5 = new QGraphicsDropShadowEffect();
    effect5->setBlurRadius(2);
    parent_->ui()->comp_errorHistogramCustomplot->setGraphicsEffect(effect5);
}

void CompensateurPage::LoadDossier() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        if (parent_->ui()->comp_dossierLineEdit->text().isEmpty() ||
            parent_->ui()->comp_dossierLabel->text().isNull()) {
            throw std::runtime_error("Dossier id not given");
        }
        const int dossier(parent_->ui()->comp_dossierLineEdit->text().toInt());
        if (!parent_->repo()->DossierExist(dossier)) {
            throw std::runtime_error("Dossier does not exist");
        }
        ClearParameters();
        Patient patient(parent_->repo()->GetPatient(dossier));
        //WritePatient(patient);
        Modulateur modulateur(parent_->repo()->GetModulateurForDossier(dossier));
        DegradeurSet degradeur_set(parent_->repo()->GetDegradeurSetForDossier(dossier));
        double range_shift_mm_tissue(material::Plexiglas2Tissue((double(modulateur.RangeShift() +
                                                                    degradeur_set.um_plexi_total()))/1000.0));
        double parcours(30.7 - range_shift_mm_tissue - material::Plexiglas2Tissue(base_thickness_));
        parent_->ui()->comp_parcoursLineEdit->setText(QString::number(parcours, 'f', 2));
    }
    catch(std::exception& exc) {
        qWarning() << QString("CompensateurPage::LoadDossier Exception thrown: ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void CompensateurPage::CreateAndDraw() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        auto compensateur(CreateCompensateur());
        Draw(compensateur);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "MTP", QString("Could not compute compensateur: ") + QString(exc.what()));
    }
}

void CompensateurPage::WritePatient(const Patient& patient) {
//    parent_->ui()->comp_patientLineEdit->setText(patient.GetLastName().toUpper() + " " +
//                                                 patient.GetFirstName());
}

void CompensateurPage::Draw(const Compensateur& compensateur) {
    try {
        compXY_graph_.Plot(compensateur);
        compXZ_graph_.Plot(compensateur);
        eyeXZ_graph_.Plot(compensateur, parent_->repo()->GetModulateurForDossier(compensateur.dossier()).mod100());
        compError_graph_.Plot(compensateur);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "MTP", QString("Could not draw compensateur: ") + QString(exc.what()));
    }
}

// can throw
Eye CompensateurPage::CreateEye() const {
    qDebug() << "Creating eye";
    if (parent_->ui()->comp_eyeDiameterLineEdit->text().isEmpty() ||
        parent_->ui()->comp_eyeDSclereLineEdit->text().isEmpty()) {
        throw std::runtime_error("Input missing, cannot create eye");
    }
    const double diameter_eye(parent_->ui()->comp_eyeDiameterLineEdit->text().toDouble());
    const double sclere_thickness(parent_->ui()->comp_eyeDSclereLineEdit->text().toDouble());
    return Eye(diameter_eye, sclere_thickness);
}

// can throw
Compensateur CompensateurPage::CreateCompensateur() const {
    qDebug() << "Creating compensateur";
    if (parent_->ui()->comp_dossierLineEdit->text().isEmpty() ||
            parent_->ui()->comp_bolusThicknessLineEdit->text().isEmpty() ||
            parent_->ui()->comp_parcoursLineEdit->text().isEmpty() ||
            parent_->ui()->comp_xcentreLineEdit->text().isEmpty() ||
            parent_->ui()->comp_ycentreLineEdit->text().isEmpty()) {
        throw std::runtime_error("Input missing, cannot create compensateur");
    }

    const int dossier(parent_->ui()->comp_dossierLineEdit->text().toInt());
    const double bolus_thickness(parent_->ui()->comp_bolusThicknessLineEdit->text().toDouble());

    const double parcours(parent_->ui()->comp_parcoursLineEdit->text().toDouble());
    const double max_parcours_error(parent_->ui()->comp_maxParcoursErrorDoubleSpinBox->value());
    const double error_scaling(((double)parent_->ui()->comp_errorScalingHorizontalSlider->value()) / 100.0);
    const double x_centre(parent_->ui()->comp_xcentreLineEdit->text().toDouble());
    const double y_centre(parent_->ui()->comp_ycentreLineEdit->text().toDouble());

    return Compensateur(dossier, CreateEye(), QCPCurveData(0, x_centre, y_centre),
                        bolus_thickness, parcours, max_parcours_error, error_scaling,
                        base_thickness_);
}

void CompensateurPage::SaveToDatabase() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        Compensateur compensateur(CreateCompensateur());
        MillingMachine milling_machine;
        milling_machine.CreateProgram(compensateur);
        parent_->repo()->SaveCompensateurItems(compensateur, milling_machine.GetProgram());
        QMessageBox::information(parent_, "MTP", QString("The compensateur was successfully saved to database"));
    }
    catch (std::exception& exc) {
        qWarning() << QString("CompensateurPage::SaveToDatabase Exception thrown: ") + exc.what();
        QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void CompensateurPage::FetchFromDatabase() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        bool user_input_ok(false);
        const int dossier = QInputDialog::getInt(parent_, tr("MTP"), "Dossier: ",
                                                 0, 0, 1000000, 1, &user_input_ok);
        if (!user_input_ok) {
            qWarning() << "CompensateurPage::FetchFromDatabase QInputDialog returned an error";
            return;
        }
        auto compensateur(parent_->repo()->GetCompensateur(dossier));
        //WritePatient(parent_->repo()->GetPatient(compensateur.dossier()));
        WriteCompensateurData(compensateur);
        Draw(compensateur);
    }
    catch (std::exception& exc) {
      qWarning() << QString("CompensateurPage::FetchFromDatabase Exception thrown: ") + exc.what();
      QMessageBox::warning(parent_, "MTP", "Exception thrown: " + QString(exc.what()));
    }
}

void CompensateurPage::WriteCompensateurData(const Compensateur &compensateur) {
    parent_->ui()->comp_dossierLineEdit->setText(QString::number(compensateur.dossier()));
    parent_->ui()->comp_bolusThicknessLineEdit->setText(QString::number(compensateur.bolus_thickness(), 'f', 2));
    parent_->ui()->comp_parcoursLineEdit->setText(QString::number(compensateur.parcours(), 'f', 2));
    parent_->ui()->comp_xcentreLineEdit->setText(QString::number(compensateur.center_point().key, 'f', 2));
    parent_->ui()->comp_ycentreLineEdit->setText(QString::number(compensateur.center_point().value, 'f', 2));
    WriteEyeData(compensateur.eye());
    parent_->ui()->comp_maxParcoursErrorDoubleSpinBox->setValue(compensateur.parcours_error());
    parent_->ui()->comp_errorScalingHorizontalSlider->setValue(compensateur.error_scaling()*100.0);
}

void CompensateurPage::WriteEyeData(const Eye &eye) {
    parent_->ui()->comp_eyeDiameterLineEdit->setText(QString::number(eye.diameter(), 'f', 2));
    parent_->ui()->comp_eyeDSclereLineEdit->setText(QString::number(eye.sclere_thickness(), 'f', 2));
}

void CompensateurPage::ShowFraiseuseProgram() {
    try {
       Compensateur compensateur(CreateCompensateur());
       MillingMachine milling_machine;
       if (!milling_machine.CreateProgram(compensateur)) {
           throw std::runtime_error("Compensateur milling machine program could not be created");
       }
       MillingMachineProgramDialog dialog(parent_, milling_machine.GetProgram());
       dialog.exec();
    }
    catch (std::exception& exc) {
     QMessageBox::warning(parent_, QString("MTP"), QString(exc.what()));
    }
}

void CompensateurPage::PrintTransparancy() {
    try {
        if (!parent_->repo()->CheckConnection()) { return; }
        Compensateur compensateur(CreateCompensateur());
        CompensateurTransparantDialog dialog(parent_, compensateur, parent_->repo()->GetPatient(compensateur.dossier()));
        dialog.exec();
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "MTP", QString("Could not compute compensateur: ") + QString(exc.what()));
    }
}
