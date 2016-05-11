void on_bntEOD1_clicked();
#ifndef MEX_ADMINPANEL_H
#define MEX_ADMINPANEL_H

#include <mex_login.h>
#include <mex_main.h>
#include <QWidget>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QtSql>

namespace Ui {
class MEX_AdminPanel;
}

class MEX_AdminPanel : public QWidget
{
    Q_OBJECT

public:
    //Constructor / Deconstructor
    explicit MEX_AdminPanel(QWidget *parent = 0); //QWidget *parent = 0
    ~MEX_AdminPanel();

    void setExchangeStatus(bool open);

signals:
    void changeExchangeStatus(QByteArray);

private slots:

    void showUsers();

    void showProducts();

    void deleteUser();

    void showUserPW();

    void changePassword();

    void refreshUserList();

    void loadProductList();

    void addProduct();

    void deleteProduct();

    QString encrypt(QString);

    QSqlQuery executeQuery(QString, bool&);

    void closeDB();

    void on_btnShowUsers_clicked();

    void on_btnShowProducts_clicked();

    void on_btnAddProduct_clicked();

    void on_btnDeleteProduct_clicked();

    void on_btnShowPW_clicked();

    void on_btnDeleteUser_clicked();

    void on_btnChangePW_clicked();

    void on_btnSOD_clicked();

    void on_btnEOD_clicked();

private:
    Ui::MEX_AdminPanel *ui;

    QSqlDatabase db;

    QStringList userList;

    QStringList productSymbolList;

    QStringList productNameList;

    QStringList productIndexList;

    QCryptographicHash* hash;

    bool open;
};

#endif // MEX_ADMINPANEL_H


