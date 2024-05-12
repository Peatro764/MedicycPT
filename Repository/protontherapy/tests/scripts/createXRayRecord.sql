CREATE TABLE XRayRecord
(
    xray_record_id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    dossier INTEGER NOT NULL,
    peak_value_sv NUMERIC,
    integrated_charge NUMERIC
);
