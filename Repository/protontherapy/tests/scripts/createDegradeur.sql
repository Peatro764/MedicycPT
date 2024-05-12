CREATE TABLE Degradeur
(
 degradeur_id INTEGER PRIMARY KEY CHECK (degradeur_id >= 0),
 um_plexi INTEGER NOT NULL CHECK (um_plexi >= 0),
 available BOOLEAN NOT NULL
);
