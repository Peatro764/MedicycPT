CREATE TABLE SensorConfig
(
    name TEXT PRIMARY KEY,
    single_ended_channel INTEGER NOT NULL,
    range NUMERIC NOT NULL,
    resolution_index INTEGER NOT NULL,
    settling_us NUMERIC NOT NULL,
    channel INTEGER NOT NULL,
    unit_per_volt NUMERIC NOT NULL,
    unit_offset NUMERIC NOT NULL -- this offset should be in the same unit as the material (thus for ex. water for the cuve)
);

CREATE TABLE MotorConfig
(
    name TEXT NOT NULL,
    type TEXT NOT NULL,
    ch_full_or_half_step INTEGER NOT NULL,
    ch_direction INTEGER NOT NULL,
    ch_preset INTEGER NOT NULL,
    ch_pulse INTEGER NOT NULL,
    low_to_high_transition_count INTEGER NOT NULL,
    high_to_low_transition_count INTEGER NOT NULL,
    n_pulses_per_step INTEGER NOT NULL,
    distance_per_step NUMERIC NOT NULL,
    distance_offset NUMERIC NOT NULL,
    msec_per_step_constants_default REAL[] NOT NULL,
    msec_per_step_constants_fast REAL[] NOT NULL,
    invert_direction BOOLEAN NOT NULL,
    PRIMARY KEY(name, type)
);

CREATE TABLE ClockConfig
(
   type TEXT NOT NULL,
   channel INTEGER CHECK (channel >= 0 AND channel <=2),
   divisor INTEGER NOT NULL CHECK (divisor > 0),
   roll_value INTEGER NOT NULL CHECK (roll_value > 0),
   PRIMARY KEY(type)
);
