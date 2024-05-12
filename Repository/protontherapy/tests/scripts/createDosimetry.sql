CREATE TABLE Dosimetry
(
    dosimetry_id SERIAL PRIMARY KEY,
    dossier INTEGER NOT NULL REFERENCES Treatment(dossier) ON UPDATE CASCADE,
    timestamp TIMESTAMP NOT NULL,
    reference BOOLEAN NOT NULL,
    monte_carlo BOOLEAN NOT NULL,
    temperature NUMERIC NOT NULL CHECK (temperature > -273.0),
    pressure NUMERIC NOT NULL CHECK (pressure > 700.0),
    chambre_id INTEGER REFERENCES Chambre(chambre_id) ON UPDATE CASCADE,
    comment VARCHAR(50)
);
