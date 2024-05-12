#include "SeanceParameterSelectorDialog.h"
#include "ui_SeanceParameterSelectorDialog.h"

#include "Util.h"

SeanceParameterSelectorDialog::SeanceParameterSelectorDialog(QWidget *parent, SeanceConfig config)
    : QDialog(parent),
      ui_(new Ui::SeanceParameterSelectorDialog),
      config_(config)
{
    ui_->setupUi(this);
    setWindowTitle("Séance Parametrès");

    SetupSpinboxes();
    SetupLimits();
    SetInitialValues();
    ConnectSignals();
}

void SeanceParameterSelectorDialog::SetupSpinboxes() {
    ui_->spinBox_mu->findChild<QLineEdit*>()->setReadOnly(true);
    ui_->spinBox_current_stripper->findChild<QLineEdit*>()->setReadOnly(true);
    ui_->doubleSpinBox_duration->findChild<QLineEdit*>()->setReadOnly(true);
}

void SeanceParameterSelectorDialog::SetupLimits() {
    ui_->spinBox_current_stripper->setMinimum(min_stripper_current_);
    ui_->spinBox_current_stripper->setMaximum(max_stripper_current_);
    ui_->verticalSlider_stripper_current->setMaximum(max_stripper_current_);
    ui_->verticalSlider_stripper_current->setMinimum(min_stripper_current_);

    ui_->spinBox_mu->setMinimum(min_um_);
    ui_->spinBox_mu->setMaximum(max_um_);
    ui_->verticalSlider_mu->setMaximum(max_um_);
    ui_->verticalSlider_mu->setMinimum(min_um_);

    ui_->doubleSpinBox_duration->setMinimum(ms2s(min_duration_));
    ui_->doubleSpinBox_duration->setMaximum(ms2s(max_duration_));
    ui_->verticalSlider_duration->setMaximum(max_duration_);
    ui_->verticalSlider_duration->setMinimum(min_duration_);
}

void SeanceParameterSelectorDialog::ConnectSignals() {
    QObject::connect(ui_->verticalSlider_mu, &QSlider::valueChanged, this, [=](int i) { SetUM(i); });
    QObject::connect(ui_->verticalSlider_duration, &QSlider::valueChanged, this, [=](int i) { SetDuration(i); });
    QObject::connect(ui_->verticalSlider_stripper_current, &QSlider::valueChanged, this, [=](int i) { SetStripperCurrent(i); });

    QObject::connect(ui_->spinBox_mu, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i){ SetUM(i); });
    QObject::connect(ui_->doubleSpinBox_duration, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](double d){ SetDuration(s2ms(d)); });
    QObject::connect(ui_->spinBox_current_stripper, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i){ SetStripperCurrent(i); });

    QObject::connect(ui_->pushButton_ok, &QPushButton::clicked, this, &QDialog::accept);
    QObject::connect(ui_->pushButton_cancel, &QPushButton::clicked, this, &QDialog::reject);
}

void SeanceParameterSelectorDialog::SetUM(int um) {
    if (static_cast<uint32_t>(um) != config_.mu_des()) {
        config_.SetMUDes(static_cast<uint32_t>(um));
        if (ui_->verticalSlider_mu->value() != um) ui_->verticalSlider_mu->setValue(um);
        if (ui_->spinBox_mu->value() != um) ui_->spinBox_mu->setValue(um);
        ui_->doubleSpinBox_dose->setValue(static_cast<double>(config_.dose()));
        if (!ui_->checkBox_decouple->isChecked()) {
            SetStripperCurrent(static_cast<int>(std::round(util::GetStripperCurrent(static_cast<double>(config_.debit()),
                                                                                    ui_->doubleSpinBox_dose->value(), ui_->doubleSpinBox_duration->value()))));
        }
    }
}

void SeanceParameterSelectorDialog::SetDuration(int ms) {
    if (static_cast<uint32_t>(ms) != config_.duration()) {
        const int duration = std::min(std::max(ms, min_duration_), max_duration_);
        config_.SetDuration(static_cast<uint32_t>(duration));
        if (ui_->verticalSlider_duration->value() != duration) ui_->verticalSlider_duration->setValue(duration);
        if (s2ms(ui_->doubleSpinBox_duration->value()) != duration) ui_->doubleSpinBox_duration->setValue(ms2s(duration));
        if (!ui_->checkBox_decouple->isChecked()) {
            SetStripperCurrent(static_cast<int>(std::round(util::GetStripperCurrent(static_cast<double>(config_.debit()),
                                                                                    ui_->doubleSpinBox_dose->value(),
                                                                                    ms2s(duration)))));
        }
    }
}

void SeanceParameterSelectorDialog::SetStripperCurrent(int nA) {
    if (static_cast<uint32_t>(nA) != config_.current_stripper()) {
        const int stripper_current = std::min(std::max(nA, min_stripper_current_), max_stripper_current_);
        config_.SetCurrentStripper(static_cast<uint32_t>(stripper_current));
        if (ui_->verticalSlider_stripper_current->value() != stripper_current) ui_->verticalSlider_stripper_current->setValue(stripper_current);
        if (ui_->spinBox_current_stripper->value() != stripper_current) ui_->spinBox_current_stripper->setValue(stripper_current);
        if (!ui_->checkBox_decouple->isChecked()) {
            SetDuration(s2ms(util::GetSeanceDuration(static_cast<double>(config_.debit()),
                                                     ui_->doubleSpinBox_dose->value(),
                                                     static_cast<double>(stripper_current))));
        }
    }
}

void SeanceParameterSelectorDialog::SetInitialValues() {
    ui_->verticalSlider_mu->setValue(config_.mu_des());
    ui_->verticalSlider_duration->setValue(config_.duration());
    ui_->verticalSlider_stripper_current->setValue(config_.current_stripper());
    ui_->spinBox_mu->setValue(config_.mu_des());
    ui_->doubleSpinBox_duration->setValue(ms2s(config_.duration()));
    ui_->spinBox_current_stripper->setValue(config_.current_stripper());
    ui_->doubleSpinBox_dose->setValue(config_.dose());
}

int SeanceParameterSelectorDialog::s2ms(double s) const {
    return static_cast<int>(1000.0 * s);
}

double SeanceParameterSelectorDialog::ms2s(int ms) const {
    return static_cast<double>(ms) / 1000.0;
}

