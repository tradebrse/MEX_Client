#include "mex_main.h"
#include "ui_mex_main.h"

MEX_Main::MEX_Main(QString userID, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MEX_Main)
{
    using namespace std;

    ui->setupUi(this);
    ///this->showMaximized();
    this->setFixedSize(this->size());


    //Set checked buttons for application start
    ui->radioButtonBuy->setChecked(true);
    ui->radioButtonAll->setChecked(true);
    ui->checkBoxAllProducts->setChecked(true);

    //connect checkbox to combobox
    connect(ui->checkBoxAllProducts, SIGNAL(clicked(bool)), ui->cBoxProductShow, SLOT(setDisabled(bool)));

    //connect column clicks to sort functions
    //Order the tables by time
    ui->tableWidgetOrderbookSell->sortItems(6);
    ui->tableWidgetOrderbookBuy->sortItems(6);
    //Connect column clicks to sort function
    connect(ui->tableWidgetOrderbookSell->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(sortSellTable(int,Qt::SortOrder)));
    connect(ui->tableWidgetOrderbookBuy->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(sortBuyTable(int,Qt::SortOrder)));

    //Delete main window when closed on logout
    this->setAttribute(Qt::WA_DeleteOnClose);

    //Set user ID
    this->userID = userID;

    //Setup DB
    QString dbPath = QApplication::applicationDirPath() + "/MEX_DB.sqlite";
    db = QSqlDatabase::addDatabase("QSQLITE", "main_connection");
    db.setDatabaseName(dbPath);

    //Load Trader data from DB
    loadTrader();

    //Generate Products
    readProductDB();


    //Set indices in
    QStringList distinctProductIndexList;
    for(int i = 0; i < productIndexList.length(); i++)
    {
        if(!distinctProductIndexList.contains(productIndexList.value(i)))
        {
            distinctProductIndexList.append(productIndexList.value(i));
            ui->cBoxIndexShow->addItem(productIndexList.value(i));
            ui->cBoxIndexExec->addItem(productIndexList.value(i));
        }
    }
    ui->cBoxIndexShow->model()->sort(0);
    ui->cBoxIndexExec->model()->sort(0);
    ui->cBoxIndexShow->setCurrentIndex(0);
    ui->cBoxIndexExec->setCurrentIndex(0);

    connect(ui->cBoxIndexExec, SIGNAL(currentTextChanged(QString)), this, SLOT(readProductDB(QString)));
    connect(ui->cBoxIndexShow, SIGNAL(currentTextChanged(QString)), this, SLOT(readProductDB(QString)));

    //Set standard GUI options for shown products and users
    selectedProducts = "ALL";
    selectedUsers = "ALL";

    //Check usertype to activate admin mode
    bool ok = false;
    QString sqlCommand = "SELECT usertype FROM userList WHERE id = '" + userID + "' ";
    QSqlQuery query(db);
    query  = executeQuery(sqlCommand, ok);

    if (ok)
    {
        query.first();
        if (query.record().value(0).toString() == "client")
        {
            ui->actionAdmin_Panel->setVisible(false);
        }
        else if(query.record().value(0).toString() == "admin")
        {
            ui->actionAdmin_Panel->setVisible(true);
        }
    } else
    {
        //Error while executing SQL-Statement
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not execute query.");
        //deactivate admin user panel
        ui->actionAdmin_Panel->setVisible(false);
        messageBox.show();
    }


    tcpClientSocket = new MEX_TCPClientSocket(traderID);

    connect(tcpClientSocket,SIGNAL(clientConnected()),this,SLOT(changeToConnected()));
    connect(tcpClientSocket,SIGNAL(clientDisconnected()),this,SLOT(changeToDisconnected()));
    connect(tcpClientSocket,SIGNAL(serverDataToGUI(QList<MEX_Order>, QList<MEX_Order>)), this,SLOT(updateOrderLists(QList<MEX_Order>, QList<MEX_Order>)));
    //Start connection
    tcpClientSocket->doConnect();
    tcpClientSocket->requestOrderbook();
}

MEX_Main::~MEX_Main()
{
    closeDB();
    delete ui;
}

//Connect to server
void MEX_Main::on_btnConnect_clicked()
{
    tcpClientSocket->doConnect();
}

//Disconnect from server
void MEX_Main::on_btnDisconnect_clicked()
{
    tcpClientSocket->doDisconnect();
}

void MEX_Main::changeToConnected()
{
    ui->lblConnectionStatus->setText("Connected");
    ui->btnConnect->setDisabled(true);
    ui->btnDisconnect->setEnabled(true);
    this->isConnected = true;
}

void MEX_Main::changeToDisconnected()
{
    ui->lblConnectionStatus->setText("Disconnected");
    ui->btnDisconnect->setDisabled(true);
    ui->btnConnect->setEnabled(true);
    this->isConnected = false;
}

void MEX_Main::updateOrderLists(QList<MEX_Order> currentOrderbook, QList<MEX_Order> matchedOrders)
{
    this->currentOrderbook = currentOrderbook;
    refreshTable();
    this->myOrders = matchedOrders;
}

void MEX_Main::on_btnLogOut_clicked()
{
    logOutUser();
}

//Close main application and open login widget
void MEX_Main::logOutUser()
{
    QWidget *loginWidget = new MEX_Login;
    loginWidget->show();
    closeDB();
    this->close();
}

void MEX_Main::on_actionAdmin_Panel_triggered()
{
    openAdminPanel();
}

//Disable main application and open 'User Panel' widget
void MEX_Main::openAdminPanel()
{
    MEX_AdminPanel *adminPanelWidget = new MEX_AdminPanel();
    adminPanelWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect( adminPanelWidget, SIGNAL(destroyed()), this, SLOT(enableWindow()));
    connect( adminPanelWidget, SIGNAL(destroyed()), this, SLOT(loadTrader()));
    connect( adminPanelWidget, SIGNAL(destroyed()), this, SLOT(readProductDB()));
    connect( this, SIGNAL(destroyed()), adminPanelWidget, SLOT(close()));
    adminPanelWidget->show();
    this->setDisabled(true);
}

void MEX_Main::on_actionMy_Account_triggered()
{
    openMyAccount();
}

//Disable main application and open 'My Account' widget
void MEX_Main::openMyAccount()
{
    MEX_MyAccount *myAccount = new MEX_MyAccount(userID);
    myAccount->setAttribute(Qt::WA_DeleteOnClose);
    connect( myAccount, SIGNAL(destroyed()), this, SLOT(enableWindow()));
    connect( myAccount, SIGNAL(destroyed()), this, SLOT(loadTrader()));
    connect( this, SIGNAL(destroyed()), myAccount, SLOT(close()));
    myAccount->show();
    this->setDisabled(true);
}

void MEX_Main::on_actionTrade_Log_triggered()
{
    openTradeLog();
}

//Disable main application and open 'tradeLog' dialog
void MEX_Main::openTradeLog(){

    //Iterator that goes through all matched orders
    QList<MEX_Order>::iterator matchedOrdersIterator;
    //Set products for all orders by looking up porduct symbol
    for(matchedOrdersIterator = myOrders.begin(); matchedOrdersIterator != myOrders.end(); matchedOrdersIterator++)
    {
        for(int i = 0; i < productList.length(); i++)
        {
            if((*matchedOrdersIterator).getProduct().getSymbol() == productList.value(i).getSymbol())
            {
                (*matchedOrdersIterator).setProduct(productList.value(i));
            }
        }
    }

    MEX_TradeLog *tradeLogDialog = new MEX_TradeLog(myOrders, this->userID);
    tradeLogDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect( tradeLogDialog, SIGNAL(destroyed()), this, SLOT(enableWindow()));
    ///Funktioniert loadTrader??
    connect( tradeLogDialog, SIGNAL(destroyed()), this, SLOT(loadTrader()));
    connect( this, SIGNAL(destroyed()), tradeLogDialog, SLOT(close()));
    tradeLogDialog->show();
    this->setDisabled(true);
}

void MEX_Main::enableWindow()
{
    this->setEnabled(true);
}

//Read products from db and load into GUI
void MEX_Main::readProductDB(QString index)
{

    if(ui->cBoxIndexShow->currentText() != index)
    {
        ui->cBoxIndexShow->blockSignals(true);
        ui->cBoxIndexShow->setCurrentText(index);
        ui->cBoxIndexShow->blockSignals(false);
    }
    else if(ui->cBoxIndexExec->currentText() != index)
    {
        ui->cBoxIndexExec->blockSignals(true);
        ui->cBoxIndexExec->setCurrentText(index);
        ui->cBoxIndexExec->blockSignals(false);
    }


    bool ok = false;
    QString sqlCommand = "SELECT symbol,name,indexName FROM productList ORDER BY name ASC";
    QSqlQuery query(db);
    query  = executeQuery(sqlCommand, ok);

    if (ok)
    {
        //Clear old lists
        productSymbolList.clear();
        productNameList.clear();
        productIndexList.clear();
        //go to first line
        ok = query.first();
        //While there is a next line:
        while (ok)
        {
            //fill product lists of symbols, names, and indices
            productSymbolList.append(query.value(0).toString());
            productNameList.append(query.value(1).toString());
            productIndexList.append(query.value(2).toString());
            ok = query.next(); //get next line
        }
    }
    else
    {
        //Error while executing SQL-Statement
        QMessageBox::critical(0,"Error","Could not execute query.");
    }
    ui->cBoxProductExec->clear();
    ui->cBoxProductShow->clear();
    generateProducts(productSymbolList, productNameList,productIndexList);
    for(int i = 0; i < productList.length(); i++)
    {
        if(productList.value(i).getIndex() == index)
        {
            ui->cBoxProductShow->addItem(productList.value(i).getName());
            ui->cBoxProductExec->addItem(productList.value(i).getName());
        }
    }
}

void MEX_Main::generateProducts(QStringList symbol, QStringList name, QStringList index)
{
    productList.clear();
    for (int i = 0; i < symbol.size(); ++i)
    {
        product.setName(name.value(i));
        product.setSymbol(symbol.value(i));
        product.setIndex(index.value(i));
        productList.append(product);
    }
}

//Load trader information from SQL DB to trader object
void MEX_Main::loadTrader()
{
    bool ok = false;
    QString   sqlCommand = "SELECT user, pass, usertype, credit FROM userList WHERE id = '" + userID + "' ";
    QSqlQuery query(db);
    query  = executeQuery(sqlCommand, ok);

    if ( ok == true )
    {
        query.first();
        traderID = userID;
        username = query.record().value(0).toString();
        password = query.record().value(1).toString();
        permission = query.record().value(2).toString();
        credit = query.record().value(3).toInt();
        trader.setTraderID(traderID);
        trader.setName(username);
        trader.setPassword(password);
        trader.setPermission(permission);
        trader.setCredit(credit);
    }
    else
    {
        //Error while executing SQL-Statement
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not execute query.");
        messageBox.show();
    }
}

void MEX_Main::on_btnExecute_clicked()
{
    if(this->isConnected)
    {
        executeOrder();
        refreshTable();
    }
    else
    {
        QMessageBox::information(0,"Execution failed","The client is not connected to the server.");
    }
}

//Gather trade information and send over tcp client
void MEX_Main::executeOrder()
{
    QString ordertype;
    if(ui->radioButtonSell->isChecked())
    {
        ordertype = "SELL";
    }
    else if(ui->radioButtonBuy->isChecked())
    {
        ordertype = "BUY";
    }

    bool ok = false;
    QString   sqlCommand = "SELECT symbol FROM productList WHERE name = '" + ui->cBoxProductExec->currentText() + "' ";
    QSqlQuery query(db);
    query  = executeQuery(sqlCommand, ok);
    QString productSymbol;
    if (ok)
    {
        query.first();
        productSymbol = query.record().value(0).toString();
    }
    else
    {
        //Error while executing SQL-Statement
        QMessageBox::critical(0,"Error","Could not execute query.");
    }
    int value = ui->edtValue->text().toInt();
    int quantity = ui->edtQuantity->text().toInt();
    QString comment = ui->edtComment->text();

    //Send order information from GUI to TCP socket
    tcpClientSocket->sendOrder(traderID, value, quantity, comment, productSymbol, ordertype);
}

//Get current filter settings and refresh the tables
void MEX_Main::on_btnShow_clicked()
{
    if(this->isConnected)
    {
        if(ui->checkBoxAllProducts->isChecked() == true)
        {
            selectedProducts = "ALL";
        }
        else
        {
            selectedProducts = ui->cBoxProductShow->currentText();
        }

        if(ui->radioButtonAll->isChecked() == true)
        {
            selectedUsers = "ALL";
        }
        else if(ui->radioButtonMine->isChecked() == true)
        {
            selectedUsers = userID;
        }
        tcpClientSocket->requestOrderbook();
    }
    else
    {
        QMessageBox::information(0,"Execution failed","The client is not connected to the server.");
    }
}

void MEX_Main::refreshTable()
{
    //Remove all current rows from tablewidget
    while (ui->tableWidgetOrderbookSell->rowCount() > 0)
    {
        ui->tableWidgetOrderbookSell->removeRow(0);
    }
    //Remove all current rows from tablewidget
    while (ui->tableWidgetOrderbookBuy->rowCount() > 0)
    {
        ui->tableWidgetOrderbookBuy->removeRow(0);
    }
    //Iterator that goes through all orders
    QList<MEX_Order>::iterator orderbookIterator;
    //Set products for all orders by looking up porduct symbol
    for(orderbookIterator = currentOrderbook.begin(); orderbookIterator != currentOrderbook.end(); orderbookIterator++)
    {
        for(int i = 0; i < productList.length(); i++)
        {
            if((*orderbookIterator).getProduct().getSymbol() == productList.value(i).getSymbol())
            {
                (*orderbookIterator).setProduct(productList.value(i));
            }
        }
    }
    //Order the tables by time
    ui->tableWidgetOrderbookSell->sortItems(6);
    ui->tableWidgetOrderbookBuy->sortItems(6);
    //seperate the orders and add the information to the table widgets
    for(orderbookIterator = currentOrderbook.begin(); orderbookIterator != currentOrderbook.end(); orderbookIterator++)
    {
        if((*orderbookIterator).getOrdertype() == "SELL")
        {
            if((selectedProducts == "ALL" || selectedProducts == (*orderbookIterator).getProduct().getName()) && (selectedUsers == "ALL" || selectedUsers == (*orderbookIterator).getTraderID()) && (ui->cBoxIndexShow->currentText() == (*orderbookIterator).getProduct().getIndex()))
            {
                //Count number of rows
                newRow = ui->tableWidgetOrderbookSell->rowCount();
                //insert new Row at end of widget
                ui->tableWidgetOrderbookSell->insertRow(newRow);
                // ui->tableWidgetOrderbookSell->setItem(
                ui->tableWidgetOrderbookSell->setItem(newRow, 0,new QTableWidgetItem((*orderbookIterator).getProduct().getSymbol()));
                ui->tableWidgetOrderbookSell->setItem(newRow, 1,new QTableWidgetItem((*orderbookIterator).getProduct().getIndex()));
                ui->tableWidgetOrderbookSell->setItem(newRow, 2,new MEX_TableWidgetItem("0"));
                ui->tableWidgetOrderbookSell->setItem(newRow, 3,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getQuantity())));
                ui->tableWidgetOrderbookSell->setItem(newRow, 4,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getValue())));
                ui->tableWidgetOrderbookSell->setItem(newRow, 5,new QTableWidgetItem((*orderbookIterator).getComment()));
                ui->tableWidgetOrderbookSell->setItem(newRow, 6,new QTableWidgetItem((*orderbookIterator).getTime().toString("hh:mm:ss.zzz")));
            }
        }
        else if((*orderbookIterator).getOrdertype() == "BUY")
        {
            if((selectedProducts == "ALL" || selectedProducts == (*orderbookIterator).getProduct().getName()) && (selectedUsers == "ALL" || selectedUsers == (*orderbookIterator).getTraderID()) && (ui->cBoxIndexShow->currentText() == (*orderbookIterator).getProduct().getIndex()))
            {
                newRow = ui->tableWidgetOrderbookBuy->rowCount();

                ui->tableWidgetOrderbookBuy->insertRow(newRow);
                ui->tableWidgetOrderbookBuy->setItem(newRow, 0,new QTableWidgetItem((*orderbookIterator).getProduct().getSymbol()));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 1,new QTableWidgetItem((*orderbookIterator).getProduct().getIndex()));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 2,new MEX_TableWidgetItem("0"));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 3,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getQuantity())));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 4,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getValue())));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 5,new QTableWidgetItem((*orderbookIterator).getComment()));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 6,new QTableWidgetItem((*orderbookIterator).getTime().toString("hh:mm:ss.zzz")));
            }
        }
    }
}

//Sort tables by given column
void MEX_Main::sortSellTable(int column, Qt::SortOrder order)
{
    ui->tableWidgetOrderbookSell->sortItems(column,order);
}

void MEX_Main::sortBuyTable(int column, Qt::SortOrder order)
{
    ui->tableWidgetOrderbookBuy->sortItems(column,order);
}

//SQL database query execution
QSqlQuery MEX_Main::executeQuery (QString sqlCommand, bool &ok)
{
    if (!db.open())
    {
        QMessageBox::critical(0,"Error","No database connection.");
        QSqlQuery emptyQuery;
        return emptyQuery;
    } else
    {
        QSqlQuery query(db);
        ok = query.exec(sqlCommand);
        return query;
    }
}

//Closing the current connection
void MEX_Main::closeDB()
{
    //Get connection name
    QString connection;
    connection = db.connectionName();
    //Close connection
    db.close();
    db = QSqlDatabase();
    //remove old connection
    db.removeDatabase(connection);
}
