CREATE TABLE Defaults
(
    defaults_id INTEGER PRIMARY KEY,
    chambre VARCHAR(30) NOT NULL,
    temperature NUMERIC NOT NULL CHECK (temperature > -273.0),
    pressure NUMERIC NOT NULL CHECK (pressure > 800.0),
    dref NUMERIC NOT NULL CHECK (dref > 0.0),
    duration_factor REAL NOT NULL CHECK (duration_factor > 0.0),
    stripper_response_correction REAL NOT NULL CHECK (stripper_response_correction > 0.0),
    seance_duration REAL NOT NULL CHECK (seance_duration > 0.0)
);
