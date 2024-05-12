#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QDebug>

class PasswordConfirmation : public QDialog
{
    Q_OBJECT

private:
    QLabel* labelPassword;
    QLineEdit* editPassword;
    QDialogButtonBox* buttons;

    void setUpGUI();

private slots:
    void Cancel();
    void CheckPassword();

public:
    explicit PasswordConfirmation(QWidget *parent, QString user, QString password);

private:
    QString user_;
    QString password_;

};
