CREATE TABLE Treatment
(
    dossier INTEGER PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    treatment_type TEXT REFERENCES TreatmentType(name) ON UPDATE CASCADE,
    patient_id INTEGER NOT NULL REFERENCES Patient(patient_id),
    active_seance INTEGER NOT NULL,
    modulateur_id INTEGER NOT NULL REFERENCES Modulateur(modulateur_id),
    pri_degradeur_id INTEGER NOT NULL REFERENCES Degradeur(degradeur_id),
    sec_degradeur_id INTEGER REFERENCES Degradeur(degradeur_id),
    comments TEXT
);
