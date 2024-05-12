#ifndef PROTONTHERAPIE_BALISECONFIGURATION_H
#define PROTONTHERAPIE_BALISECONFIGURATION_H

#include <QDateTime>

class BaliseConfiguration {

public:
    BaliseConfiguration() {}
    BaliseConfiguration(QDateTime timestamp, int mode_functionnement, int preampli_type, int preampli_gamme, bool instantaneous_lam,
                        bool integrated1_lam, bool integrated2_lam, bool command_confirmation, bool include_date_in_measurement,
                        bool include_raw_in_measurement, int instantaneous_time, int instantaneous_elements,
                        int integrated1_time, int integrated2_time, int number_of_measurements,
                        double instantaneous_conversion_coefficient, double integrated_conversion_coefficient, double integrated_threshold,
                        double threshold_A, double threshold_B, double threshold_C)
        : timestamp_(timestamp), mode_functionnement_(mode_functionnement), preampli_type_(preampli_type), preampli_gamme_(preampli_gamme), instantaneous_lam_(instantaneous_lam),
          integrated1_lam_(integrated1_lam), integrated2_lam_(integrated2_lam), command_confirmation_(command_confirmation),
          include_date_in_measurement_(include_date_in_measurement), include_raw_in_measurement_(include_raw_in_measurement),
          instantaneous_time_(instantaneous_time), instantaneous_elements_(instantaneous_elements), integrated1_time_(integrated1_time),
          integrated2_time_(integrated2_time), number_of_measurements_(number_of_measurements), instantaneous_conversion_coefficient_(instantaneous_conversion_coefficient),
          integrated_conversion_coefficient_(integrated_conversion_coefficient), integrated_threshold_(integrated_threshold),
          threshold_A_(threshold_A), threshold_B_(threshold_B), threshold_C_(threshold_C) {}
    ~BaliseConfiguration() {}

    QDateTime Timestamp() const { return timestamp_; }
    int ModeFunctionnement() const { return mode_functionnement_; }
    int PreAmpliType() const { return preampli_type_; }
    int PreAmpliGamme() const { return preampli_gamme_; }
    bool InstantaneousLAM() const { return instantaneous_lam_; }
    bool Integrated1LAM() const { return integrated1_lam_; }
    bool Integrated2LAM() const { return integrated2_lam_; }
    bool CommandConfirmation() const { return command_confirmation_; }
    bool IncludeDateInMeasurement() const { return include_date_in_measurement_; }
    bool IncludeRawInMeasurement() const { return include_raw_in_measurement_; }
    int InstantaneousTime() const { return instantaneous_time_; }
    int InstantaneousElements() const { return instantaneous_elements_; }
    int Integrated1Time() const { return integrated1_time_; }
    int Integrated2Time() const { return integrated2_time_; }
    int NumberOfMeasurements() const { return number_of_measurements_; }
    double InstantaneousConversionCoefficient() const { return instantaneous_conversion_coefficient_; }
    double IntegratedConversionCoefficient() const { return integrated_conversion_coefficient_; }
    double IntegratedThreshold() const { return integrated_threshold_; }
    double ThresholdA() const { return threshold_A_; }
    double ThresholdB() const { return threshold_B_; }
    double ThresholdC() const { return threshold_C_; }


private:
    QDateTime timestamp_;
    int mode_functionnement_ = -1;
    int preampli_type_ = -1;
    int preampli_gamme_ = -1;
    bool instantaneous_lam_ = false;
    bool integrated1_lam_ = false;
    bool integrated2_lam_ = false;
    bool command_confirmation_ = false;
    bool include_date_in_measurement_ = false;
    bool include_raw_in_measurement_ = false;
    int instantaneous_time_ = -1;
    int instantaneous_elements_ = -1;
    int integrated1_time_ = -1;
    int integrated2_time_ = 1;
    int number_of_measurements_ = -1;
    double instantaneous_conversion_coefficient_ = -1.0;
    double integrated_conversion_coefficient_ = -1.0;
    double integrated_threshold_ = -1.0;
    double threshold_A_ = -1.0;
    double threshold_B_ = -1.0;
    double threshold_C_ = -1.0;
};

#endif
