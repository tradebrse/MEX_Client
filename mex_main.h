#ifndef MEX_MAIN_H
#define MEX_MAIN_H

#include <ui_mex_main.h>
#include <mex_tcpclientsocket.h>
#include <mex_login.h>
#include <mex_adminpanel.h>
#include <mex_myaccount.h>
#include <mex_trader.h>
#include <mex_order.h>
#include <mex_tradelog.h>
#include <QMainWindow>
#include <QDebug>
#include <QVarLengthArray>
#include <QtSql>

namespace Ui {
class MEX_Main;
}

class MEX_Main : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor/Deconstructor
    explicit MEX_Main(QString, QWidget *parent = 0);
    ~MEX_Main();

signals:

private slots:

    void on_btnLogOut_clicked();

    void logOutUser();

    void openAdminPanel();

    void openMyAccount();

    void on_actionAdmin_Panel_triggered();

    void on_actionMy_Account_triggered();

    void closeDB();

    void enableWindow();

    QSqlQuery executeQuery(QString, bool&);

    void readProductDB();

    void generateProducts(QStringList, QStringList, QStringList);

    void loadTrader();

    void on_btnExecute_clicked();

    void executeOrder();

    void refreshTable();

    void on_btnShow_clicked();

    void on_actionTrade_Log_triggered();

    void openTradeLog();

    void on_btnConnect_clicked();

    void on_btnDisconnect_clicked();

    void changeToConnected();

    void changeToDisconnected();

    void updateOrderbook(QList<MEX_Order> currentOrderbook);

private:
    Ui::MEX_Main *ui;

    QSqlDatabase db;

    MEX_TCPClientSocket *tcpClientSocket;

    QString userID;

    QStringList productNameList;

    QStringList productSymbolList;

    QStringList productIndexList;

    QList<MEX_Product> productList;

    MEX_Trader trader;

    QString traderID;

    QString username;

    QString password;

    QString permission;

    int credit;

    QList<MEX_Order*> matchedOrders;

    QList<MEX_Order> currentOrderbook;

    MEX_Product product;

    int newRow;

    QVarLengthArray<int> ordersToDelete;

    //Return if client server connection is established
    bool isConnected;

    QString selectedProducts;
    QString selectedUsers;
protected:

};

#endif // MEX_MAIN_H
