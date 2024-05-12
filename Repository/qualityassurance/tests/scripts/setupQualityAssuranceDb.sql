create database qualityassurance;
\connect qualityassurance;

\i createConstants.sql;
\i createDevices.sql;
\i createBeamMeasurement.sql;

\i fillDevices.sql;

grant SELECT, INSERT, UPDATE on all tables in schema public to pt_client;
grant usage on all sequences in schema public to pt_client;

grant all on all tables in schema public to petter;
grant all on all sequences in schema public to petter;

