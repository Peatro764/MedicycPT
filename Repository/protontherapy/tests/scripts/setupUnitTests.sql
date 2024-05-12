INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, um_degradeur, um_plexi_weight, available, opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod, decalage, conv_criteria) VALUES(10000,100.0,100.0,100.0, 0, '{"(0,1.0)"}', true, 'Joel99', 'Shifted bragg', 4, 0.8, 20.0, 10.0, 0.4, 0.001);
INSERT INTO Modulateur(modulateur_id, modulation_100, modulation_98, parcours, um_degradeur, um_plexi_weight, available, opt_alg, data_set, n_sectors, mod_step, input_parcours, input_mod, decalage, conv_criteria) VALUES(10001,100.0,100.0,100.0, 0, '{"(0,1.0)"}', true, 'Joel99', 'Shifted bragg', 4, 0.8, 20.0, 10.0, 0.4, 0.001);
INSERT INTO Degradeur(degradeur_id,um_plexi,available) VALUES(10000, 100000, true);
INSERT INTO Degradeur(degradeur_id,um_plexi,available) VALUES(20000, 200000, true);
INSERT INTO Degradeur(degradeur_id,um_plexi,available) VALUES(30000, 300000, true);
INSERT INTO Degradeur(degradeur_id,um_plexi,available) VALUES(40000, 400000, true);
INSERT INTO Degradeur(degradeur_id,um_plexi,available) VALUES(50000, 500000, true);
INSERT INTO Chambre(timestamp, name, nd, kqq0, yoffset, zoffset) VALUES ('2017-01-01', 'FWT-849', 25.940, 1.006, 11, 22);
INSERT INTO Chambre(timestamp, name, nd, kqq0, yoffset, zoffset) VALUES ('2017-01-01', 'PTW-593', 31.090, 1.030, 11, 22);

INSERT INTO Defaults(defaults_id, chambre, temperature, pressure, dref, duration_factor, stripper_response_correction, seance_duration) VALUES (1, 'FWT-849', 20.0, 1013, 1.37, 125.0, 1.0, 10.0);
INSERT INTO Patient(patient_id, first_name, last_name) VALUES(10000, 'REF', 'REF');
INSERT INTO Treatment(dossier, timestamp, patient_id, active_seance, modulateur_id, pri_degradeur_id, sec_degradeur_id) VALUES(10000, '1970-01-01', 10000, -1, 10000, 10000, 10000);
INSERT INTO baliseconfiguration(timestamp, mode_functionnement, preampli_type, preampli_gamme, instantaneous_lam, integrated1_lam,
integrated2_lam, command_confirmation, include_date_in_measurement, include_raw_in_measurement, instantaneous_time, instantaneous_elements,
integrated1_time, integrated2_time, number_of_measurements, instantaneous_conversion_coefficient, integrated_conversion_coefficient, integrated_threshold, threshold_A, threshold_B, threshold_C) VALUES('2017-01-01', 0, 1, 1, true, false, false, true, true, true, 1, 2, 60, 24, 0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
insert into treatmenttype(name, dose_seance_std, dose_seance_boost) values ('t1', '{5.2, 5.2, 5.2, 5.2, 5.2, 5.2}', '{6.9, 6.9}');
insert into treatmenttype(name, dose_seance_std, dose_seance_boost) values ('t2', '{5.2, 5.2, 5.2, 5.2, 5.2, 5.2}', '{6.9, 6.9}');
insert into checkconnection(timestamp) values('1970-01-01');




