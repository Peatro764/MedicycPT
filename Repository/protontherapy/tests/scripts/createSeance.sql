CREATE TABLE Seance
(
    seance_id SERIAL PRIMARY KEY,
    dossier INTEGER NOT NULL REFERENCES Treatment(dossier) ON UPDATE CASCADE,
    dose_prescribed NUMERIC NOT NULL CHECK (dose_prescribed >= 0.0)
);
