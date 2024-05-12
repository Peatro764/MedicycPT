CREATE TABLE BaliseRecord
(
    balise_record_id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    dossier INTEGER NOT NULL REFERENCES Treatment(dossier) ON UPDATE CASCADE,
    seance_record_id INTEGER NOT NULL REFERENCES SeanceRecord(seance_record_id) ON UPDATE CASCADE,
    charge NUMERIC NOT NULL CHECK (charge >= 0.0),
    ref_debit NUMERIC NOT NULL CHECK (ref_debit > 0.0),
    tdj_debit NUMERIC NOT NULL CHECK (tdj_debit > 0.0),
    patient_debit NUMERIC NOT NULL CHECK (patient_debit > 0.0),
    dose_gy NUMERIC NOT NULL CHECK (dose_gy > 0.0)
);
