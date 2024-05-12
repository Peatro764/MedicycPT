CREATE TABLE Compensateur
(
    compensateur_id SERIAL PRIMARY KEY,
    dossier INTEGER NOT NULL REFERENCES Treatment(dossier) ON UPDATE CASCADE,
    diameter_eye REAL NOT NULL CHECK (diameter_eye > 0.0),
    sclere_thickness REAL NOT NULL CHECK (sclere_thickness > 0.0),
    center_point POINT NOT NULL,
    bolus_thickness REAL NOT NULL CHECK (bolus_thickness >= 0.0),
    parcours REAL NOT NULL CHECK (parcours > 0.0),
    parcours_error REAL NOT NULL CHECK (parcours_error > 0.0),
    error_radius_scaling REAL NOT NULL CHECK (error_radius_scaling >= 0.001 AND error_radius_scaling <= 1.0),
    base_thickness REAL NOT NULL CHECK (base_thickness >= 0.0)
);
