#ifndef PROTONTHERAPIE_PATIENT_H
#define PROTONTHERAPIE_PATIENT_H

#include <QString>

class Patient {
public:
    Patient(const int& id,
            const QString& first_name,
            const QString& last_name);
    Patient(const QString& first_name,
            const QString& last_name);
    Patient();
    ~Patient();
    int GetId() const { return id_; }
    QString GetFirstName() const { return first_name_; }
    QString GetLastName() const { return last_name_; }
    bool SameNameAs(const Patient& patient) const;

private:
    int id_;
    QString first_name_;
    QString last_name_;
};

bool operator==(const Patient &p1, const Patient& p2);

#endif // PROTONTHERAPIE_PATIENT_H
