#include "ConfigDialog.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QCoreApplication>
#include <QStandardPaths>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent)
{
    QGridLayout *mainLayout = new QGridLayout;

    settings_ = new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("MMM.ini"), QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    auto groups = settings_->childGroups();
    int row(0);
    foreach (auto group, groups) {
        settings_->beginGroup(group);
        auto keys = settings_->childKeys();
        foreach (auto key, keys) {
            QString groupAndKey(group + "/" + key);
            lineEdits_.insert(groupAndKey, new QLineEdit(settings_->value(key).toString()));
            mainLayout->addWidget(new QLabel(groupAndKey), row, 0);
            mainLayout->addWidget(lineEdits_.value(groupAndKey), row++, 1);
        }
        settings_->endGroup();
    }
    mainLayout->addWidget(new QLabel("ini file location"), row, 0);
    mainLayout->addWidget(new QLabel(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).join("\n")), row++, 1);

    buttonBox_ = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox_, row++, 0);

    setLayout(mainLayout);
    setWindowTitle(tr("Configuration"));

    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(Save()));
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(Cancel()));
}

void ConfigDialog::Save()
{
    auto groups = settings_->childGroups();
    foreach (auto group, groups) {
        settings_->beginGroup(group);
        auto keys = settings_->childKeys();
        foreach (auto key, keys) {
            QString groupAndKey(group + "/" + key);
            settings_->setValue(key, lineEdits_.value(groupAndKey)->text());
        }
        settings_->endGroup();
    }
    hide();
    accept();
}

void ConfigDialog::Cancel() {
    auto groups = settings_->childGroups();
    foreach (auto group, groups) {
        settings_->beginGroup(group);
        auto keys = settings_->childKeys();
        foreach (auto key, keys) {
            QString groupAndKey(group + "/" + key);
            lineEdits_.value(groupAndKey)->setText(settings_->value(key).toString());
        }
        settings_->endGroup();
    }
    hide();
    reject();
}
