CREATE TABLE BaliseInstantaneousLevel
(
    balise_level_id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    level REAL NOT NULL
);

CREATE TABLE BaliseIntegratedLevel
(
    balise_level_id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    level REAL NOT NULL
);

CREATE TABLE BaliseBufferLevel
(
    balise_level_id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    level REAL NOT NULL
);
