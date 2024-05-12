CREATE TABLE MeasurementCurrents
(
  measurement_currents_id SERIAL PRIMARY KEY,
  timestamp TIMESTAMP NOT NULL,
  measurement_type TEXT NOT NULL,
  measurement_id INTEGER NOT NULL,
  i_stripper REAL NOT NULL,
  i_cf9 REAL NOT NULL,
  i_chambre1 REAL NOT NULL,
  i_chambre2 REAL NOT NULL,
  i_diode REAL NOT NULL
);

CREATE TABLE MeasurementPoint
(
  id SERIAL PRIMARY KEY,
  measurement_type TEXT NOT NULL,
  measurement_id INTEGER NOT NULL,
  x REAL NOT NULL,
  y REAL NOT NULL,
  z REAL NOT NULL,
  i_diode REAL NOT NULL,
  i_chambre REAL NOT NULL
);

CREATE TABLE BeamProfile
(
  beam_profile_id SERIAL PRIMARY KEY,
  timestamp TIMESTAMP NOT NULL,
  hardware HARDWARE NOT NULL,
  axis AXIS NOT NULL,
  smoothing_elements INTEGER NOT NULL,
  centre REAL NOT NULL,
  width_95 REAL NOT NULL,
  width_90 REAL NOT NULL,
  penumbra REAL NOT NULL,
  flatness REAL NOT NULL,
  noise REAL NOT NULL
);

CREATE TABLE BraggPeak
(
  bragg_peak_id SERIAL PRIMARY KEY,
  timestamp TIMESTAMP NOT NULL,
  hardware HARDWARE NOT NULL,
  smoothing_elements INTEGER NOT NULL,
  width_50 REAL NOT NULL,
  penumbra REAL NOT NULL,
  parcours REAL NOT NULL,
  noise REAL NOT NULL
);

CREATE TABLE SOBP
(
  sobp_id SERIAL PRIMARY KEY,
  timestamp TIMESTAMP NOT NULL,
  hardware HARDWARE NOT NULL,
  smoothing_elements INTEGER NOT NULL,
  dossier INTEGER,
  modulateur_id INTEGER NOT NULL,
  degradeur_mm REAL NOT NULL,
  comment TEXT,
  penumbra REAL NOT NULL,
  parcours REAL NOT NULL,
  modulation_98 REAL,
  modulation_100 REAL,
  noise REAL NOT NULL
);

CREATE TABLE CUBE
(
  cube_id SERIAL PRIMARY KEY,
  timestamp TIMESTAMP NOT NULL,
  comment TEXT,
  x_nbins INTEGER NOT NULL CHECK (x_nbins > 0),
  x_min REAL NOT NULL,
  x_step REAL NOT NULL CHECK (x_step >= 0.0),
  y_nbins INTEGER NOT NULL CHECK (y_nbins > 0),
  y_min REAL NOT NULL,
  y_step REAL NOT NULL CHECK (y_step >= 0.0),
  z_nbins INTEGER NOT NULL CHECK (z_nbins > 0),
  z_min REAL NOT NULL,
  z_step REAL NOT NULL CHECK (z_step >= 0.0),
  noise REAL NOT NULL CHECK (noise >= 0.0)
);

