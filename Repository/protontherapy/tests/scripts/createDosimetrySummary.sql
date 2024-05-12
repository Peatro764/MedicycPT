CREATE TABLE DosimetrySummary
(
    dosimetry_summary_id SERIAL PRIMARY KEY,
    dossier INTEGER REFERENCES Treatment(dossier) ON UPDATE CASCADE,
    timestamp TIMESTAMP NOT NULL,
    reference BOOLEAN NOT NULL,
    monte_carlo BOOLEAN NOT NULL,
    chambre_id INTEGER REFERENCES Chambre(chambre_id) ON UPDATE CASCADE,
    debit NUMERIC NOT NULL CHECK (debit >= 0.0),
    debit_stddev REAL NOT NULL CHECK (debit_stddev >= 0.0)
);
