#ifndef MEX_LOGIN_H
#define MEX_LOGIN_H

#include <mex_main.h>
#include <QWidget>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QtSql>
#include <QNetworkProxy>

namespace Ui {
class MEX_Login;
}

class MEX_Login : public QWidget
{
    Q_OBJECT

public:
    // Constructor/Deconstructor
    explicit MEX_Login(QWidget *parent = 0);
    ~MEX_Login();

    void setUserID(QString);

    QString getUserID();

signals:

private slots:

    void on_btnLogIn_clicked();

    void on_btnSignUp_clicked();

    void loadUserList();

    void logInUser();

    void registerUser();

    void openMainWindow(QString);

    void closeDB();

    QSqlQuery executeQuery(QString, bool&);

    QString encrypt(QString);

private:
    Ui::MEX_Login *ui;

    QString userID;

    QCryptographicHash* hash;

    // SQL-DB //
    QSqlDatabase db;

    // Username List //
    QStringList userList;

    QNetworkProxy proxy;
};

#endif // MEX_LOGIN_H
