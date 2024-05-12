create database protontherapy;
\connect protontherapy;

\i createOMRON.sql;
\i createPatient.sql;
\i createModulateur.sql;
\i createDegradeur.sql;
\i createMCNPXDoseCurve.sql
\i createTheoreticalDoseCurve.sql
\i createTreatment.sql;
\i createChambre.sql;
\i createFraiseuseProgram.sql;
\i createCollimateur.sql;
\i createCompensateur.sql;
\i createSeance.sql;
\i createDosimetrySummary.sql;
\i createDosimetry.sql;
\i createDosimetryRecord.sql
\i createSeanceRecord.sql;
\i createBaliseLevel.sql;
\i createBaliseRecord.sql;
\i createBaliseCalibration.sql;
\i createBaliseConfiguration.sql;
\i createXRayRecord.sql;
\i createDefaults.sql;
\i createOperator.sql;
\i createActiveTreatment.sql;
\i createOIDS.sql;
\i createTriggers.sql;
\i createCheckConnection.sql;

\i fillOMRON.sql;
\i fillChambres.sql;
\i fillDegradeurs.sql;
\i fillModulateurs.sql;
\i fillMCNPXDoseCurve.sql
\i fillTheoreticalDoseCurve.sql
\i fillOperators.sql;
\i fillDefaults.sql;
\i fillTopDuJourDossier.sql;
\i fillCheckConnection.sql;

grant SELECT, INSERT, UPDATE on all tables in schema public to pt_client;
grant usage on all sequences in schema public to pt_client;

grant all on all tables in schema public to petter;
grant all on all sequences in schema public to petter;

grant all on all tables in schema public to marc;
grant all on all sequences in schema public to marc;

grant all on all tables in schema public to joel;
grant all on all sequences in schema public to joel;

grant all on all tables in schema public to gangelli;
grant all on all sequences in schema public to gangelli;

grant SELECT, INSERT on TreatmentFraiseuseProgram to todd;
grant SELECT, INSERT on MiscFraiseuseProgram to todd;
grant SELECT,UPDATE on miscfraiseuseprogram_fraiseuse_program_id_seq to todd;
grant SELECT,UPDATE on treatmentfraiseuseprogram_fraiseuse_program_id_seq to todd;

grant USAGE on SCHEMA public to backup;
grant SELECT on all tables in schema public to backup;
grant SELECT on all sequences in schema public to backup;
