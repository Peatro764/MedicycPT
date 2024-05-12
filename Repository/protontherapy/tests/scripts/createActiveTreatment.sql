CREATE TABLE ActiveTreatment
(
    active_treatment_id SERIAL PRIMARY KEY,
    dossier INTEGER NOT NULL REFERENCES Treatment(dossier) ON UPDATE CASCADE,
    timestamp TIMESTAMP NOT NULL,
    started BOOLEAN NOT NULL
);
