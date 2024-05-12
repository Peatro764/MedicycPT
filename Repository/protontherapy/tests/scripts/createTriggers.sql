CREATE OR REPLACE FUNCTION send_fraiseuse_notification()
  RETURNS trigger AS $send_fraiseuse_notification$
  DECLARE
  msg TEXT;
  BEGIN
  msg := cast(NEW.dossier as TEXT) || '  ' || cast(NEW.program_type as TEXT);
  execute 'notify fraiseuse_notification, ''' || msg || '''';
  RETURN NEW;
 END;
 $send_fraiseuse_notification$ LANGUAGE plpgsql;

CREATE TRIGGER send_fraiseuse_notification
  AFTER INSERT ON TreatmentFraiseuseProgram
  FOR EACH ROW
  EXECUTE PROCEDURE send_fraiseuse_notification();

CREATE OR REPLACE FUNCTION send_seancerecord_notification()
  RETURNS trigger AS $send_seancerecord_notification$
  DECLARE
  msg TEXT;
  BEGIN
  msg := cast(NEW.seance_record_id as TEXT);
  execute 'notify seancerecord_notification, ''' || msg || '''';
  RETURN NEW;
 END;
 $send_seancerecord_notification$ LANGUAGE plpgsql;

CREATE TRIGGER send_seancerecord_notification
  AFTER INSERT ON SeanceRecord
  FOR EACH ROW
  EXECUTE PROCEDURE send_seancerecord_notification();

CREATE OR REPLACE FUNCTION send_activetreatment_notification()
  RETURNS trigger AS $send_activetreatment_notification$
  DECLARE
  msg TEXT;
  BEGIN
  msg := cast(NEW.dossier as TEXT);
  execute 'notify activetreatment_notification, ''' || msg || '''';
  RETURN NEW;
 END;
 $send_activetreatment_notification$ LANGUAGE plpgsql;

CREATE TRIGGER send_activetreatment_notification
  AFTER INSERT ON ActiveTreatment
  FOR EACH ROW
  EXECUTE PROCEDURE send_activetreatment_notification();

CREATE OR REPLACE FUNCTION send_treatmentstatus_notification()
  RETURNS trigger AS $send_treatmentstatus_notification$
  DECLARE
  msg TEXT;
  BEGIN
  msg := cast(NEW.dossier as TEXT);
  IF NEW.started THEN
  execute 'notify treatmentstarted_notification, ''' || msg || '''';
  ELSE
  execute 'notify treatmentcancelled_notification, ''' || msg || '''';
  END IF;
  RETURN NEW;
 END;
 $send_treatmentstatus_notification$ LANGUAGE plpgsql;

CREATE TRIGGER send_treatmentstatus_notification
  AFTER UPDATE ON ActiveTreatment
  FOR EACH ROW
  EXECUTE PROCEDURE send_treatmentstatus_notification();
