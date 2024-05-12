INSERT INTO SensorConfig VALUES('DIODE', 199, 10.0, 8, 0, 0, 1.0, 0.0);
INSERT INTO SensorConfig VALUES('CHAMBRE', 199, 10.0, 8, 0, 1, 1.0, 0.0);
INSERT INTO SensorConfig VALUES('X', 199, 10.0, 8, 0, 2, -10.17153, 56.68186);
INSERT INTO SensorConfig VALUES('Y', 199, 10.0, 8, 0, 3, -10.222365, 51.15475);
INSERT INTO SensorConfig VALUES('Z', 199, 10.0, 8, 0, 4, 5.09648, -3.71992);

INSERT INTO MotorConfig VALUES('X', 'SCANNER3D', 0, 5, 1, 2, 100, 0, 10, 0.05, 0.0, '{20}', '{20}', false);
INSERT INTO MotorConfig VALUES('Y', 'SCANNER3D', 0, 6, 1, 3, 100, 0, 10, 0.05, 0.0, '{20}', '{20}', true);
INSERT INTO MotorConfig VALUES('Z', 'SCANNER3D', 0, 7, 1, 4, 100, 0, 10, 0.05, 1.780, '{20}', '{20}', false);
INSERT INTO MotorConfig VALUES('Z', 'WHEEL', 0, 7, 1, 4, 4375, 0, 2, 0.175, 1.780,  '{750, -7}', '{150}', false);
INSERT INTO MotorConfig VALUES('X', 'SCANNER2D', 0, 5, 1, 2, 100, 0, 10, 0.05, 0.0, '{20}', '{20}', true);
INSERT INTO MotorConfig VALUES('Y', 'SCANNER2D', 0, 6, 1, 3, 100, 0, 10, 0.05, 0.0, '{20}', '{20}', false);
INSERT INTO MotorConfig VALUES('Z', 'SCANNER2D', 0, 7, 1, 4, 100, 0, 10, 0.05, 0.0, '{20}', '{20}', false);

INSERT INTO ClockConfig VALUES('SCANNER2D', 1, 8, 15000);
INSERT INTO ClockConfig VALUES('SCANNER3D', 1, 8, 15000);
INSERT INTO ClockConfig VALUES('WHEEL', 1, 256, 21500);
