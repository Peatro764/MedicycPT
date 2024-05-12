#include "MathUtilsPage.h"

#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QtMath>

#include "ui_MTP.h"
#include "MTP.h"
#include "Calc.h"

MathUtilsPage::MathUtilsPage(MTP* parent)
    : parent_(parent)
{
    SetupLineEditValidators();
    SetupDefaults();
    ConnectSignals();
}

MathUtilsPage::~MathUtilsPage()
{
}

void MathUtilsPage::SetupLineEditValidators() {
    parent_->ui()->mathUtilsPage_scanPolareLineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->mathUtilsPage_scanPolareLineEdit));
    parent_->ui()->mathUtilsPage_scanAzLineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->mathUtilsPage_scanAzLineEdit));
    parent_->ui()->mathUtilsPage_scanOverHeadLineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->mathUtilsPage_scanOverHeadLineEdit));
    parent_->ui()->mathUtilsPage_scanLateralLineEdit->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->mathUtilsPage_scanLateralLineEdit));

    parent_->ui()->mathUtilsPage_coordDiametreLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->mathUtilsPage_coordDiametreLineEdit));
    parent_->ui()->mathUtilsPage_coordMesureArcLineEdit->setValidator(new QRegExpValidator(QRegExp("[+]?\\d{0,}\\.{0,1}\\d{0,}"), parent_->ui()->mathUtilsPage_coordMesureArcLineEdit));
}

void MathUtilsPage::ConnectSignals() {
    QObject::connect(parent_->ui()->mathUtilsPage_calculatePushButton, SIGNAL(clicked()), this, SLOT(CalculateNIF()));
    QObject::connect(parent_->ui()->mathUtilsPage_coordCalculatePushButton, SIGNAL(clicked()), this, SLOT(CalculateCOORD()));
    QObject::connect(parent_->ui()->mathUtilsPage_coordClearPushButton, SIGNAL(clicked()), parent_->ui()->mathUtilsPage_coordPlainTextEdit, SLOT(clear()));
}

void MathUtilsPage::SetupDefaults() {
}

void MathUtilsPage::CalculateNIF() {
    parent_->ui()->mathUtilsPage_azFinalLabelValue->setText("-");
    parent_->ui()->mathUtilsPage_polareFinaleLabelValue->setText("-");
    parent_->ui()->mathUtilsPage_nifAzLabelValue->setText("-");
    parent_->ui()->mathUtilsPage_nifPolareLabelValue->setText("-");

    if (parent_->ui()->mathUtilsPage_scanPolareLineEdit->text().isEmpty() ||
            parent_->ui()->mathUtilsPage_scanAzLineEdit->text().isEmpty() ||
            parent_->ui()->mathUtilsPage_scanOverHeadLineEdit->text().isEmpty() ||
            parent_->ui()->mathUtilsPage_scanLateralLineEdit->text().isEmpty()) {
        QMessageBox::warning(parent_, "MTP", "Au moins une variable est manquante");
        return;
    }

    try {
        const double scan_polare(parent_->ui()->mathUtilsPage_scanPolareLineEdit->text().toDouble());
        const double scan_az(parent_->ui()->mathUtilsPage_scanAzLineEdit->text().toDouble());
        const double scan_overhead(parent_->ui()->mathUtilsPage_scanOverHeadLineEdit->text().toDouble());
        const double scan_lateral(parent_->ui()->mathUtilsPage_scanLateralLineEdit->text().toDouble());

        double teta(0.0);
        double rayon = sqrt(pow(nif_dcp * qTan(qDegreesToRadians(scan_lateral)), 2) + pow(nif_dcp * qTan(qDegreesToRadians(scan_overhead)), 2));


        if (calc::AlmostEqual(scan_overhead, 0.0, 0.0001)) {
            if ((scan_lateral < 0.0 && scan_lateral > -180.0) || (scan_lateral > 180.0 && scan_lateral < 360.0)) {
                teta = 270.0;
            } else {
                teta = 90.0;
            }
        }

        if (calc::AlmostEqual(scan_lateral, 0.0, 0.0001)) {
            if ((scan_overhead < 0.0 && scan_overhead > -180.0) || (scan_overhead > 180.0 && scan_overhead < 360.0)) {
                teta = 180.0;
            } else {
                teta = 0.0;
            }
        }

        if (!calc::AlmostEqual(scan_overhead, 0.0, 0.0001) && !calc::AlmostEqual(scan_lateral, 0.0, 0.0001)) {
            teta = qRadiansToDegrees(qAtan2(qTan(qDegreesToRadians(scan_lateral)), qTan(qDegreesToRadians(scan_overhead))));
            if (teta < 0.0) teta += 360.0;
        }

        const double arg1 = nif_dcp * (qTan(qDegreesToRadians(scan_lateral)) + qTan(qDegreesToRadians(scan_polare)) * qSin(qDegreesToRadians(scan_az)));
        const double arg2 = nif_dcp * (qTan(qDegreesToRadians(scan_overhead)) + qTan(qDegreesToRadians(scan_polare)) * qCos(qDegreesToRadians(scan_az)));
        double az_final = qRadiansToDegrees(qAtan2(arg1, arg2));
        if (az_final < 0.0) az_final += 360.0;
        double polare_final = qRadiansToDegrees(qAtan2(sqrt(pow(arg1, 2) + pow(arg2, 2)), nif_dcp));
        parent_->ui()->mathUtilsPage_azFinalLabelValue->setText(QString::number(az_final, 'f', 1));
        parent_->ui()->mathUtilsPage_polareFinaleLabelValue->setText(QString::number(polare_final, 'f', 1));
        parent_->ui()->mathUtilsPage_nifAzLabelValue->setText(QString::number(teta, 'f', 1));
        parent_->ui()->mathUtilsPage_nifPolareLabelValue->setText(QString::number(rayon, 'f', 1));
    }
    catch (std::exception& exc) {
        QMessageBox::warning(parent_, "MTP", "An exception was thrown: " + QString(exc.what()));
        return;
    }
}

void MathUtilsPage::CalculateCOORD() {
    if (parent_->ui()->mathUtilsPage_coordDiametreLineEdit->text().isNull() || parent_->ui()->mathUtilsPage_coordDiametreLineEdit->text().isEmpty()) {
        QMessageBox::warning(parent_, "MTP", "Champ de diamètre vide");
        return;
    }

    const double diameter_globe(parent_->ui()->mathUtilsPage_coordDiametreLineEdit->text().toDouble());
    if (diameter_globe <= 0.0) {
        QMessageBox::warning(parent_, "MTP", "Diamètre zéro ou négatif");
        return;
    }

    if (parent_->ui()->mathUtilsPage_coordMesureArcLineEdit->text().isNull() || parent_->ui()->mathUtilsPage_coordMesureArcLineEdit->text().isEmpty()) {
        QMessageBox::warning(parent_, "MTP", "Champ de mesure d'arc vide");
        return;
    }

    const double arc(parent_->ui()->mathUtilsPage_coordMesureArcLineEdit->text().toDouble());
    if (arc <= 0.0) {
        QMessageBox::warning(parent_, "MTP", "Mesure d'arc zéro ou négative");
        return;
    }

    const double coord(diameter_globe * qSin(arc / diameter_globe));
    QString msg = QString("Arc = ") + QString::number(arc, 'f', 2) + QString("  -> ") + QString::number(coord, 'f', 2);
    parent_->ui()->mathUtilsPage_coordPlainTextEdit->appendPlainText(msg);
}

