CREATE TABLE SeanceRecord
(
    seance_record_id SERIAL PRIMARY KEY,
    seance_id INTEGER NOT NULL REFERENCES Seance(seance_id) ON UPDATE CASCADE,
    timestamp TIMESTAMP NOT NULL,
    debit NUMERIC NOT NULL CHECK (debit >= 0.0),
    duration REAL[] NOT NULL,
    i_chambre1 REAL[] NOT NULL,
    i_chambre2 REAL[] NOT NULL,
    cf9_status INTEGER[] NOT NULL,
    i_stripper REAL NOT NULL,
    i_cf9 REAL NOT NULL,
    um_prevu INTEGER NOT NULL CHECK (um_prevu >= 0),
    um_corr INTEGER NOT NULL CHECK (um_corr >= 0),
    um_del_1 INTEGER[] NOT NULL CHECK(0 <= ALL(um_del_1)),
    um_del_2 INTEGER[] NOT NULL CHECK(0 <= ALL(um_del_2)),
    est_dose NUMERIC NOT NULL CHECK (est_dose >= 0.0) DEFAULT 0.0
);
