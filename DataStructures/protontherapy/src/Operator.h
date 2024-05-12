#ifndef PROTONTHERAPIE_OPERATOR_H
#define PROTONTHERAPIE_OPERATOR_H

#include <QString>

class Operator {
public:
    Operator(const QString& username, const QString& password) : username_(username), password_(password) {}
    ~Operator() {}
    QString GetUsername() { return username_; }
    QString GetPassword() { return password_; }

private:
    QString username_;
    QString password_;
};

#endif // PROTONTHERAPIE_OPERATOR_H
