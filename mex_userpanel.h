#ifndef MEX_USERPANEL_H
#define MEX_USERPANEL_H

#include <mex_login.h>
#include <mex_main.h>
#include <QWidget>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QtSql>

namespace Ui {
class MEX_UserPanel;
}

class MEX_UserPanel : public QWidget
{
    Q_OBJECT

public:
    //Constructor / Deconstructor
    explicit MEX_UserPanel(QWidget *parent = 0); //QWidget *parent = 0
    ~MEX_UserPanel();

private slots:

    void showUsers();

    void deleteUser();

    void showUserPW();

    void changePassword();

    void refreshList();

    QString encrypt(QString);

    QSqlQuery executeQuery(QString, bool&);

    void closeDB();

    void on_btnShowUsers_clicked();

    void on_btnShowPW_clicked();

    void on_btnDeleteUser_clicked();

    void on_btnChangePW_clicked();

private:
    Ui::MEX_UserPanel *ui;

    QSqlDatabase db;

    QStringList userList;

    QCryptographicHash* hash;
};

#endif // MEX_USERPANEL_H
