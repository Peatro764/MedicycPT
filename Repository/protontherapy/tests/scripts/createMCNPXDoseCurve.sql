CREATE TABLE MCNPXDoseCurve
(
  um_plexi INTEGER PRIMARY KEY,
  monitor_units REAL NOT NULL CHECK (monitor_units >= 0.0),
  depth_dose_array POINT[] NOT NULL CHECK (POINT(-0.0001, -0.0001) << ALL(depth_dose_array) AND POINT(-0.0001, -0.0001) <^ ALL(depth_dose_array))
);

