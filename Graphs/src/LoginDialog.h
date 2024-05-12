#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QDebug>

class LoginDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* labelUsername;
    QLabel* labelPassword;
    QComboBox* comboUsername;
    QLineEdit* editPassword;
    QDialogButtonBox* buttons;

    void setUpGUI();

public:
    explicit LoginDialog(QWidget *parent = 0);
    void setUsernamesList( const QStringList& usernames );
    void Show() {this->setVisible(true); this->show();}

signals:
    void acceptLogin( QString& username, QString& password, int& indexNumber );

public slots:
    void slotAcceptLogin();
    void LoginOk();

};
