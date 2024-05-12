#ifndef MTP_H
#define MTP_H

#include <QSettings>
#include <QWidget>

#include "CollimateurPage.h"
#include "CompensateurPage.h"
#include "PTRepo.h"
#include "QARepo.h"
#include "DossierPage.h"
#include "LibraryPage.h"
#include "LoginDialog.h"
#include "ModulateurPage.h"
#include "MathUtilsPage.h"

namespace Ui {
class MTP;
}

class MTP : public QWidget
{
    Q_OBJECT

public:
    explicit MTP(QWidget *parent = 0);
    ~MTP();
    Ui::MTP* ui() { return ui_; }
    PTRepo* repo() { return pt_repo_; }
    QARepo* qa_repo() { return qa_repo_; }

public slots:
    void GotoTopMenu();
    void GotoLeftMenu();
    void GotoRightMenu();
    void ShowConfigurationDialog();
    void SetupDb();

    void AcceptUserLogin(QString& userName, QString& password);
    void LoginLogout();

    void ShowDossierWidget();
    void ShowModDegLibWidget();
    void ShowCollimateurWidget();
    void ShowCompensateurWidget();
    void ShowModulateurWidget();
    void ShowMathUtilsWidget();
    void UpdateTitle(int index);

private slots:
    void LaunchPrintDialog(int dossier);
    void TurnOffDbButton();
    void TurnOnDbButton();
    void FailedConnectingToDatabase();

private:
    void SetupConnections();
    void SetupLineValidators();
    void SetupDefaults();
    QString ButtonStyleSheet(QString image);

    void GetUsers();
    void DisableProtectedSections(bool disable);

    Ui::MTP *ui_;
    QSettings settings_;
    DossierPage* dossier_page_;
    LibraryPage* library_page_;
    CollimateurPage* collimateur_page_;
    CompensateurPage* compensateur_page_;
    ModulateurPage *modulateur_page_;
    MathUtilsPage *math_util_page_;
    PTRepo *pt_repo_;
    QARepo *qa_repo_;
    LoginDialog login_dialog_;

    bool logged_in_;

    enum TabWidget { START, DOSSIER, MODDEGLIB, MATHUTIL, COLLIMATEUR, COMPENSATEUR, MODULATEUR };
};


#endif // MTP_H
