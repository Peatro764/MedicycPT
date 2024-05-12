create database unittests;
\connect unittests

DROP SCHEMA public cascade;
CREATE SCHEMA public AUTHORIZATION hofverberg
grant all on all tables in schema public to hofverberg;
grant all on all sequences in schema public to hofverberg;
grant all on schema public to hofverberg;





