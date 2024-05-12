CREATE TABLE BaliseCalibration
(
    balise_calibration_id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    sv_per_coulomb NUMERIC NOT NULL CHECK (sv_per_coulomb > 0.0),
    doseRateToH_alpha NUMERIC NOT NULL CHECK (doseRateToH_alpha > 0.0),
    doseRateToH_beta NUMERIC NOT NULL CHECK (doseRateToH_beta < 0.0),
    HToDoseRate_alpha NUMERIC NOT NULL CHECK (HToDoseRate_alpha > 0.0),
    HToDoseRate_beta NUMERIC NOT NULL CHECK (HToDoseRate_beta < 0.0)    
);
