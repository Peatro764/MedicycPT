
CREATE TABLE omron (
    nom_equipement character(20) NOT NULL,
    nom_usage character varying(30) NOT NULL,
    type integer,
    ip character(17) NOT NULL,
    port integer NOT NULL,
    idserv smallint NOT NULL,
    online smallint NOT NULL,
    eqt_num integer,
    actuation integer NOT NULL,
    quittance integer NOT NULL,
    address integer NOT NULL,
    acq_driver integer,
    acq_unit character(4),
    acq_lsb numeric(7,6),
    value numeric(7,2),
    acq_unit2 character(4),
    acq_lsb2 numeric(7,6),
    value2 numeric(7,2),
    acq_max numeric(7,2),
    acq_min numeric(7,2),
    ref_acq_proton numeric(7,2),
    ref_acq_f18 numeric(7,2),
    ref_acq_neutron numeric(7,2),
    ref_acq_extension numeric(7,2),
    ref_acq_test numeric(7,2),
    cont_driver integer,
    cont_lsb numeric(7,6),
    cont_max numeric(7,2),
    cont_min numeric(7,2),
    ref_cont_proton numeric(7,2),
    ref_cont_f18 numeric(7,2),
    ref_cont_neutron numeric(7,2),
    ref_cont_extension numeric(7,2),
    ref_cont_test numeric(7,2),
    reference numeric(7,2),
    type_ns character(10) NOT NULL,
    type_mes integer NOT NULL,
    polarite integer NOT NULL,
    filtre integer NOT NULL,
    flag_surv integer NOT NULL,
    flag_surv_i integer NOT NULL,
    tolerance numeric(3,2)
);

ALTER TABLE ONLY omron
    ADD CONSTRAINT omron_nom_equipement_key UNIQUE (nom_equipement);

COMMENT ON COLUMN omron.type IS 'Omron= 6,7,8 Barco=9';






