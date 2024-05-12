#include "PasswordConfirmation.h"

PasswordConfirmation::PasswordConfirmation(QWidget *parent, QString user, QString password) :
    QDialog(parent), user_(user), password_(password)
{
    setUpGUI();
    setWindowTitle(QString("Mot de passe ") + user_);
//    this->setVisible(true);
//    this->show();
}

void PasswordConfirmation::setUpGUI(){
    QGridLayout* formGridLayout = new QGridLayout( this );

    // initialize the password field so that it does not echo characters
    editPassword = new QLineEdit( this );
    editPassword->setEchoMode( QLineEdit::Password );

    labelPassword = new QLabel( this );
    labelPassword->setText( tr( "Password" ) );
    labelPassword->setBuddy( editPassword );

    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Ok") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Annuler") );

    connect(buttons->button( QDialogButtonBox::Cancel ), SIGNAL (clicked()),
               this, SLOT (reject()));


    QObject::connect(buttons->button( QDialogButtonBox::Cancel ), SIGNAL(clicked()), this, SLOT(Cancel()));
    QObject::connect(buttons->button( QDialogButtonBox::Ok ), SIGNAL(clicked()), this, SLOT(CheckPassword()));

    // place components into the dialog
    formGridLayout->addWidget( labelPassword, 0, 0 );
    formGridLayout->addWidget( editPassword, 0, 1 );
    formGridLayout->addWidget( buttons, 2, 0, 1, 2 );

    setLayout( formGridLayout );
}

void PasswordConfirmation::CheckPassword() {
    if (editPassword->text() == password_) {
        done(1);
    } else {
        done(0);
    }
}

void PasswordConfirmation::Cancel() {
    done(2);
}

