#ifndef MEX_MYACCOUNT_H
#define MEX_MYACCOUNT_H

#include <mex_main.h>
#include <QMainWindow>
#include <QWidget>
#include <QMessageBox>
#include <QtSql>
#include <QDebug>

namespace Ui {
class MEX_MyAccount;
}

class MEX_MyAccount : public QWidget
{
    Q_OBJECT

public:
    //Constructor / Deconstructor

    class MEX_Main;

    explicit MEX_MyAccount(QString userID, QWidget *parent = 0);
    ~MEX_MyAccount();

    //Setter methods
    void setUserID(QString);

private slots:
    void on_btnChangePW_clicked();

    void changePassword();

    void closeDB();

    QString encrypt(QString);

    QSqlQuery executeQuery(QString, bool&);

private:

    QString username;
    QString email;
    QString credit;
    QString usertype;
    QString currentUserPass;

    Ui::MEX_MyAccount *ui;

    QSqlDatabase db;

    QString userID;

};

#endif // MEX_MYACCOUNT_H
