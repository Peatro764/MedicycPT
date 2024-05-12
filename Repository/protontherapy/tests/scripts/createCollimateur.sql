CREATE TABLE Collimateur
(
    collimateur_id SERIAL PRIMARY KEY,
    dossier INTEGER NOT NULL REFERENCES Treatment(dossier) ON UPDATE CASCADE,
    internal_edges PATH NOT NULL
);
