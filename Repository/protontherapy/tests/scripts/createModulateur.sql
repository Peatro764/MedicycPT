CREATE TABLE Modulateur
(
  modulateur_id INTEGER PRIMARY KEY,
  modulation_100 REAL NOT NULL CHECK (modulation_100 >= 0.0),
  modulation_98 REAL NOT NULL CHECK (modulation_98 >= 0.0),
  parcours REAL NOT NULL CHECK (parcours >= 0.0),
  um_degradeur INTEGER NOT NULL CHECK (um_degradeur >= 0),
  um_plexi_weight POINT[] NOT NULL CHECK (POINT(-0.0001, -0.0001) << ALL(um_plexi_weight) AND POINT(-0.0001, -0.0001) <^ ALL(um_plexi_weight)),
  available BOOLEAN NOT NULL,
  opt_alg TEXT NOT NULL,
  data_set TEXT NOT NULL,
  n_sectors INTEGER NOT NULL CHECK (n_sectors > 0),
  mod_step REAL NOT NULL CHECK (mod_step > 0.0),
  input_parcours REAL NOT NULL CHECK (input_parcours >= 0.0),
  input_mod REAL NOT NULL CHECK (input_mod >= 0.0),
  decalage REAL NOT NULL CHECK (decalage >= 0.0),
  conv_criteria REAL NOT NULL CHECK (conv_criteria > 0.0)
);
