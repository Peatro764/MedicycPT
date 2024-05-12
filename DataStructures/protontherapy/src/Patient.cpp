#include "Patient.h"

Patient::Patient(const QString& first_name,
                 const QString& last_name)
    : id_(-1),
      first_name_(first_name),
      last_name_(last_name) {}

Patient::Patient(const int& id,
                 const QString& first_name,
                 const QString& last_name)
    : id_(id),
      first_name_(first_name),
      last_name_(last_name) {}

Patient::Patient() :
    id_(0),
    first_name_(""),
    last_name_("") {}

Patient::~Patient() {}

bool Patient::SameNameAs(const Patient &patient) const {
    return (first_name_.toLower() == patient.GetFirstName().toLower() &&
            last_name_.toLower() == patient.GetLastName().toLower());
}

bool operator==(const Patient &p1, const Patient& p2) {
    return (p1.GetId() == p2.GetId() &&
            p1.GetFirstName() == p2.GetFirstName() &&
            p1.GetLastName() == p2.GetLastName());
}


