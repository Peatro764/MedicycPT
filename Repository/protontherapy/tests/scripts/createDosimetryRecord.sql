CREATE TABLE DosimetryRecord
(
    dosimetry_record_id SERIAL PRIMARY KEY,
    dosimetry_id INTEGER NOT NULL REFERENCES Dosimetry(dosimetry_id) ON UPDATE CASCADE,
    duration REAL[] NOT NULL,
    i_chambre1 REAL[] NOT NULL,
    i_chambre2 REAL[] NOT NULL,
    i_stripper REAL NOT NULL,
    i_cf9 REAL NOT NULL,
    um_prevu INTEGER NOT NULL CHECK (um_prevu >= 0),
    um_del INTEGER[] NOT NULL CHECK (0 <= ALL(um_del)),
    charge NUMERIC NOT NULL CHECK (charge >= 0.0),
    air_ic_charge NUMERIC NOT NULL CHECK (air_ic_charge >= 0.0) DEFAULT 0.0
);
