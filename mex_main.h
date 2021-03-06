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
#include <mex_tablewidgetitem.h>
#include <QMainWindow>
#include <QDebug>
#include <QVarLengthArray>
#include <QtSql>
#include <QRegExp>
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QCalendarWidget>

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

    void readProductDB(QString index = "DAX");

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

    void changeExchangeStatus(bool);

    void updateOrderLists(QList<MEX_Order> currentOrderbook, QList<MEX_Order> matchedOrders);

    void sortSellTable(int column, Qt::SortOrder order);

    void sortBuyTable(int column, Qt::SortOrder order);

    void logOrder(QString ordertype, QString productIndex, QString productsymbol, int quantity, double value, QString comment, bool persistent);

    void logOrder(QString ordertype, QString productIndex, QString productsymbol, int quantity, double value, QString comment, QString gtdString, bool persistent);

    void clearTables();

    void intitializeLogFile();

    void customMenuRequested(QPoint pos);

    void cancelOrder();

    void openCalendarDialog(bool active);

    void setGTD(QDate date);

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

    QList<MEX_Order> myOrders;

    QDateTime date;

    QFile logFile;

    QElapsedTimer timer;

    bool open;

    QTableWidgetItem *currentItem;

    QDate serverDate;

protected:

};

#endif // MEX_MAIN_H
