CREATE TABLE TreatmentType
(
    name TEXT NOT NULL PRIMARY KEY,
    dose_seance_std REAL[] NOT NULL CHECK (0 < ALL(dose_seance_std)),
    dose_seance_boost REAL[] CHECK (0 < ALL(dose_seance_std))
);
