CREATE TABLE Chambre
(
    chambre_id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    name VARCHAR(30) NOT NULL,
    nd NUMERIC NOT NULL CHECK (nd >= 0.0),
    kqq0 NUMERIC NOT NULL CHECK (kqq0 >= 0.0),
    yoffset NUMERIC NOT NULL,
    zoffset NUMERIC NOT NULL
);
