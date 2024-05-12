#include "LoginDialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent)
{
    setUpGUI();
    setWindowTitle( tr("Vérification du mot de passe ") );
//    setModal( true );
//    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::Dialog);
}

void LoginDialog::setUpGUI(){
    QGridLayout* formGridLayout = new QGridLayout( this );

    comboUsername = new QComboBox( this );
    comboUsername->setEditable( true );
    // initialize the password field so that it does not echo characters
    editPassword = new QLineEdit( this );
    editPassword->setEchoMode( QLineEdit::Password );

    labelUsername = new QLabel( this );
    labelPassword = new QLabel( this );
    labelUsername->setText( tr( "Username" ) );
    labelUsername->setBuddy( comboUsername );
    labelPassword->setText( tr( "Password" ) );
    labelPassword->setBuddy( editPassword );

    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Login") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

    connect(buttons->button( QDialogButtonBox::Cancel ), SIGNAL (clicked()),
            this, SLOT (reject()));

    connect( buttons->button( QDialogButtonBox::Ok ),
             SIGNAL (clicked()),
             this,
             SLOT (slotAcceptLogin()) );

    // place components into the dialog
    formGridLayout->addWidget( labelUsername, 0, 0 );
    formGridLayout->addWidget( comboUsername, 0, 1 );
    formGridLayout->addWidget( labelPassword, 1, 0 );
    formGridLayout->addWidget( editPassword, 1, 1 );
    formGridLayout->addWidget( buttons, 2, 0, 1, 2 );

    setLayout( formGridLayout );
}

void LoginDialog::slotAcceptLogin(){
    QString username = comboUsername->currentText();
    QString password = editPassword->text();
    int index = comboUsername->currentIndex();
    editPassword->clear();

    // close this dialog
    //    setVisible(false);
//    hide();
    emit acceptLogin( username, // current username
                      password, // current password
                      index // index in the username list
                      );

}

void LoginDialog::LoginOk() {
    hide();
    accept();
}

void LoginDialog::setUsernamesList(const QStringList &usernames){
    comboUsername->addItems( usernames );
}
