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

    //Set MEX Logo
    QPixmap pixMEX(QApplication::applicationDirPath() + "/mex1.png");
    ui->lblTradingGUI->setPixmap(pixMEX);

    //Set checked buttons for application start
    ui->radioButtonBuy->setChecked(true);
    ui->radioButtonAll->setChecked(true);
    ui->checkBoxAllProducts->setChecked(true);

    //Set connection label color
    ui->lblExchangeStatus->setStyleSheet("QLabel {color : red;}");

    //connect checkbox to combobox
    connect(ui->checkBoxAllProducts, SIGNAL(clicked(bool)), ui->cBoxProductShow, SLOT(setDisabled(bool)));

    //Connect GTD checkbox to calender dialog and set line edit Read Only
    connect(ui->cBoxGTD, SIGNAL(toggled(bool)),this, SLOT(openCalendarDialog(bool)));
    ui->edtDate->setReadOnly(true);

    //connect column clicks to sort functions
    //Order the tables by time
    ui->tableWidgetOrderbookSell->sortItems(6);
    ui->tableWidgetOrderbookBuy->sortItems(6);
    //Connect column clicks to sort function
    connect(ui->tableWidgetOrderbookSell->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(sortSellTable(int,Qt::SortOrder)));
    connect(ui->tableWidgetOrderbookBuy->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(sortBuyTable(int,Qt::SortOrder)));

    //Create contextmenu for table widgets | BUY Table
    QVBoxLayout *layout=new QVBoxLayout(); //(this)
    ui->tableWidgetOrderbookBuy->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetOrderbookSell->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetOrderbookBuy, SIGNAL(customContextMenuRequested(QPoint)),SLOT(customMenuRequested(QPoint)));
    connect(ui->tableWidgetOrderbookSell, SIGNAL(customContextMenuRequested(QPoint)),SLOT(customMenuRequested(QPoint)));
    layout->addWidget(ui->tableWidgetOrderbookBuy);
    layout->addWidget(ui->tableWidgetOrderbookSell);

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
    if (permission == "admin")
    {

        ui->actionAdmin_Panel->setVisible(true);
    }
    else
    {
        ui->actionAdmin_Panel->setVisible(false);
    }

    //Inititialize log file and clear old content
    intitializeLogFile();
    logFile.resize(0);

    tcpClientSocket = new MEX_TCPClientSocket(traderID);

    connect(tcpClientSocket,SIGNAL(clientConnected()),this,SLOT(changeToConnected()));
    connect(tcpClientSocket,SIGNAL(clientDisconnected()),this,SLOT(changeToDisconnected()));
    connect(tcpClientSocket,SIGNAL(exchangeStatusChanged(bool)),this,SLOT(changeExchangeStatus(bool)));
    connect(tcpClientSocket,SIGNAL(serverDataToGUI(QList<MEX_Order>, QList<MEX_Order>)), this,SLOT(updateOrderLists(QList<MEX_Order>, QList<MEX_Order>)));

    //Start connection
    tcpClientSocket->doConnect();
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
    ui->lblConnectionStatus->setStyleSheet("QLabel {color : green;}");
    ui->btnConnect->setDisabled(true);
    ui->btnDisconnect->setEnabled(true);
    this->isConnected = true;
}

void MEX_Main::changeToDisconnected()
{
    clearTables();
    changeExchangeStatus(false);
    ui->lblConnectionStatus->setText("Disconnected");
    ui->lblConnectionStatus->setStyleSheet("QLabel {color : red;}");
    ui->btnDisconnect->setDisabled(true);
    ui->btnConnect->setEnabled(true);
    this->isConnected = false;
}

void MEX_Main::openCalendarDialog(bool active)
{
    if(active)
    {
        dialogGTD = new QDialog(this);
        dialogGTD->setWindowTitle("Calendar");
        calendar = new QCalendarWidget(dialogGTD);
        connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(setGTD(QDate)));
        QVBoxLayout *extensionLayout = new QVBoxLayout;
        extensionLayout->setMargin(0);
        extensionLayout->addWidget(calendar);
        dialogGTD->setLayout(extensionLayout);
        dialogGTD->show();
        ui->edtDate->setText(QDate::currentDate().toString(Qt::ISODate));
    }
    else
    {
        delete calendar;
        delete dialogGTD;
        ui->edtDate->setText("");
    }
}

void MEX_Main::setGTD(QDate date)
{
    ui->edtDate->setText(date.toString(Qt::ISODate));
}

//Set current exchange status
void MEX_Main::changeExchangeStatus(bool open)
{
    this->open = open;
    if(open)
    {
        ui->lblExchangeStatus->setStyleSheet("QLabel {color : green;}");
        ui->lblExchangeStatus->setText("Trading open");
    }
    else
    {
        ui->lblExchangeStatus->setStyleSheet("QLabel {color : red;}");
        ui->lblExchangeStatus->setText("Trading closed");
    }
}

void MEX_Main::updateOrderLists(QList<MEX_Order> currentOrderbook, QList<MEX_Order> matchedOrders)
{
    this->currentOrderbook = currentOrderbook;
    refreshTable();
    this->myOrders = matchedOrders;

    //End timer
    if(timer.isValid())
    {
        qint64 nanoSec;
        nanoSec = timer.nsecsElapsed();
        double ms;
        ms = nanoSec/1000000;
        qDebug() << ms << " ms | " << nanoSec << " nanoseconds" << endl;
    }
}

void MEX_Main::on_btnLogOut_clicked()
{
    tcpClientSocket->doDisconnect();
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
    connect(adminPanelWidget, SIGNAL(changeExchangeStatus(QByteArray)), tcpClientSocket, SLOT(writeRawData(QByteArray)));
    adminPanelWidget->show();
    adminPanelWidget->setExchangeStatus(open);
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
    if(this->isConnected && ui->lblExchangeStatus->text() == "Trading open")
    {
        //Start timestamp timer
        timer.start();

        executeOrder();
        refreshTable();
    }
    else if(!this->isConnected)
    {
        QMessageBox::information(0,"Execution failed","The client is not connected to the server.");
    }
    else if(this->isConnected && ui->lblExchangeStatus->text() == "Trading closed")
    {
        QMessageBox::information(0,"Execution failed","The exchange is currently closed");
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
    QString   sqlCommand = "SELECT symbol, indexName FROM productList WHERE name = '" + ui->cBoxProductExec->currentText() + "' ";
    QSqlQuery query(db);
    query  = executeQuery(sqlCommand, ok);
    QString productSymbol;
    QString productIndex;
    if (ok)
    {
        query.first();
        productSymbol = query.record().value(0).toString();
        productIndex = query.record().value(1).toString();
    }
    else
    {
        //Error while executing SQL-Statement
        QMessageBox::critical(0,"Error","Could not execute query.");
    }

    //Set regular expressions for value and quantity
    QRegExp quantityRegEx("[1-9]{1}[0-9]{0,3}");
    QRegExp valueRegEx("([0-9]{1,5}[\\.][0-9]{1,3})|([1-9]{1}[0-9]{0,3})");
    QDate dateGTD;
    bool dateValid = false;

    if(ui->cBoxGTD->isChecked())
    {
        dateGTD = QDate::fromString(ui->edtDate->text(),Qt::ISODate);
        if(dateGTD < QDate::currentDate())
        {
            QMessageBox::information(0,"Invalid input","Inavlid date.");
        }
        else
        {
            dateValid = true;
        }
    }

    if(valueRegEx.exactMatch(ui->edtValue->text().replace(",",".")) && quantityRegEx.exactMatch(ui->edtQuantity->text()) && dateValid && isConnected)
    {
        double value = ui->edtValue->text().replace(",",".").toDouble();
        int quantity = ui->edtQuantity->text().toInt();
        QString comment = ui->edtComment->text();
        QString stringGTD = ui->edtDate->text();

        logOrder(ordertype, productIndex, productSymbol, quantity, value, comment, stringGTD);

        tcpClientSocket->sendOrder(traderID, value, quantity, comment, productSymbol, ordertype, stringGTD);
    }
    else if(valueRegEx.exactMatch(ui->edtValue->text().replace(",",".")) && quantityRegEx.exactMatch(ui->edtQuantity->text()) && !ui->cBoxGTD->isChecked() && isConnected)
    {
        double value = ui->edtValue->text().replace(",",".").toDouble();
        int quantity = ui->edtQuantity->text().toInt();
        QString comment = ui->edtComment->text();

        logOrder(ordertype, productIndex, productSymbol, quantity, value, comment);

        tcpClientSocket->sendOrder(traderID, value, quantity, comment, productSymbol, ordertype);
    }
    else if(!valueRegEx.exactMatch(ui->edtValue->text()))
    {
        QMessageBox::information(0,"Invalid input","Inavlid value.");
    }
    else if(!quantityRegEx.exactMatch(ui->edtQuantity->text()))
    {
        QMessageBox::information(0,"Invalid input","Invalid quantity.");
    }
}

//Send the cancel order to the server
void MEX_Main::cancelOrder()
{
    if(currentItem->isSelected() && currentItem->tableWidget()->item(currentItem->row(),2)->text().toInt() > 0)
    {
        tcpClientSocket->writeRawData(("Cancel_"+currentItem->tableWidget()->item(currentItem->row(),2)->text()).toUtf8());
    }
}

void MEX_Main::intitializeLogFile()
{
    QString filename = username+"_"+date.currentDateTime().toString("yyyy.MM.dd");
    logFile.setFileName(filename+".log");
}

//Write Order to user logfile
void MEX_Main::logOrder(QString ordertype, QString productIndex, QString productsymbol, int quantity, double value, QString comment)
{
    QString output = date.currentDateTime().toString()+" - "+ordertype+" | "+productIndex+" | "+productsymbol+" | "+QString::number(quantity)+"@"+QString::number(value)+" | Comment: "+comment+"\n";
    logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream logStream(&logFile);
    logStream<<output<<flush;
    logFile.close();
}
//With GTD
void MEX_Main::logOrder(QString ordertype, QString productIndex, QString productsymbol, int quantity, double value, QString comment, QString gtdString)
{
    QString output = date.currentDateTime().toString()+" - "+ordertype+" | "+productIndex+" | "+productsymbol+" | "+QString::number(quantity)+"@"+QString::number(value)+" | GTD: "+gtdString+" | Comment: "+comment+"\n";
    logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream logStream(&logFile);
    logStream<<output<<flush;
    logFile.close();
}

//Get current filter settings and refresh the tables
void MEX_Main::on_btnShow_clicked()
{
    if(this->isConnected)
    {
        if(ui->checkBoxAllProducts->isChecked())
        {
            selectedProducts = "ALL";
        }
        else
        {
            selectedProducts = ui->cBoxProductShow->currentText();
        }

        if(ui->radioButtonAll->isChecked())
        {
            selectedUsers = "ALL";
        }
        else if(ui->radioButtonMine->isChecked())
        {
            selectedUsers = userID;
        }
        if(isConnected)
        {
            tcpClientSocket->requestOrderbook();
        }
    }
    else
    {
        QMessageBox::information(0,"Execution failed","The client is not connected to the server.");
    }
}

void MEX_Main::clearTables()
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
}

void MEX_Main::refreshTable()
{
    clearTables();
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
                //Insert new Row at end of widget
                ui->tableWidgetOrderbookSell->insertRow(newRow);
                ui->tableWidgetOrderbookSell->setItem(newRow, 0,new QTableWidgetItem((*orderbookIterator).getProduct().getIndex()));
                ui->tableWidgetOrderbookSell->setItem(newRow, 1,new QTableWidgetItem((*orderbookIterator).getProduct().getSymbol()));
                //Set order ID if only user orders selected
                if(selectedUsers == (*orderbookIterator).getTraderID())
                {
                    ui->tableWidgetOrderbookSell->setHorizontalHeaderItem(2,new QTableWidgetItem("Order ID"));
                    ui->tableWidgetOrderbookSell->setItem(newRow, 2,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getOrderID())));
                }
                else
                {
                    ui->tableWidgetOrderbookSell->setHorizontalHeaderItem(2,new QTableWidgetItem("GTD"));
                    ui->tableWidgetOrderbookSell->setItem(newRow, 2,new QTableWidgetItem((*orderbookIterator).getGTD()));
                }
                ui->tableWidgetOrderbookSell->setItem(newRow, 3,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getQuantity())));
                ui->tableWidgetOrderbookSell->setItem(newRow, 4,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getValue())));
                ui->tableWidgetOrderbookSell->setItem(newRow, 5,new QTableWidgetItem((*orderbookIterator).getComment()));
                ui->tableWidgetOrderbookSell->setItem(newRow, 6,new QTableWidgetItem((*orderbookIterator).getTime().toString("hh:mm:ss.zzz")));
                //Highlight updated items
                if((*orderbookIterator).getUpdated() != 0)
                {
                    //Highlight all columns of the row / order
                    if((*orderbookIterator).getUpdated() == 7)
                    {
                        for(int i = 0; i < 7; i++)
                            ui->tableWidgetOrderbookSell->item(newRow,i)->setBackgroundColor(QColor(0,255,0,120));
                    }
                    //Highlight quantity only
                    else
                    {
                        ui->tableWidgetOrderbookSell->item(newRow,(*orderbookIterator).getUpdated())->setBackgroundColor(QColor(0,255,0,120));
                    }
                }
            }
        }
        else if((*orderbookIterator).getOrdertype() == "BUY")
        {
            if((selectedProducts == "ALL" || selectedProducts == (*orderbookIterator).getProduct().getName()) && (selectedUsers == "ALL" || selectedUsers == (*orderbookIterator).getTraderID()) && (ui->cBoxIndexShow->currentText() == (*orderbookIterator).getProduct().getIndex()))
            {
                newRow = ui->tableWidgetOrderbookBuy->rowCount();

                ui->tableWidgetOrderbookBuy->insertRow(newRow);
                ui->tableWidgetOrderbookBuy->setItem(newRow, 0,new QTableWidgetItem((*orderbookIterator).getProduct().getIndex()));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 1,new QTableWidgetItem((*orderbookIterator).getProduct().getSymbol()));
                if(selectedUsers == (*orderbookIterator).getTraderID())
                {
                    ui->tableWidgetOrderbookBuy->setHorizontalHeaderItem(2,new QTableWidgetItem("Order ID"));
                    ui->tableWidgetOrderbookBuy->setItem(newRow, 2,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getOrderID())));
                }
                else
                {
                    ui->tableWidgetOrderbookBuy->setHorizontalHeaderItem(2,new QTableWidgetItem("GTD"));
                    ui->tableWidgetOrderbookBuy->setItem(newRow, 2,new QTableWidgetItem((*orderbookIterator).getGTD()));
                }
                ui->tableWidgetOrderbookBuy->setItem(newRow, 3,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getQuantity())));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 4,new MEX_TableWidgetItem(QString::number((*orderbookIterator).getValue())));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 5,new QTableWidgetItem((*orderbookIterator).getComment()));
                ui->tableWidgetOrderbookBuy->setItem(newRow, 6,new QTableWidgetItem((*orderbookIterator).getTime().toString("hh:mm:ss.zzz")));
                if((*orderbookIterator).getUpdated() != 0)
                {
                    if((*orderbookIterator).getUpdated() == 7)
                    {
                        for(int i = 0; i < 7; i++)
                        {
                            ui->tableWidgetOrderbookBuy->item(newRow,i)->setBackgroundColor(QColor(0,255,0,120));
                        }
                    }
                    else
                    {
                        ui->tableWidgetOrderbookBuy->item(newRow,(*orderbookIterator).getUpdated())->setBackgroundColor(QColor(0,255,0,120));
                    }
                }
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

//Activate contextmenu
void MEX_Main::customMenuRequested(QPoint pos){
    if(selectedUsers != "ALL")
    {
        //Set contextmenu for Buy Widget
        if(this->focusWidget()->objectName() == "tableWidgetOrderbookBuy" && ui->tableWidgetOrderbookBuy->selectedItems().length() == 1)
        {
            QModelIndex index=ui->tableWidgetOrderbookBuy->indexAt(pos);

            menu =new QMenu(this);
            cancelOrderAction = new QAction(QIcon(QApplication::applicationDirPath() + "/cancel.png"),"Cancel Order", this);
            menu->addAction(cancelOrderAction);
            menu->popup(ui->tableWidgetOrderbookBuy->viewport()->mapToGlobal(pos));

            currentItem = ui->tableWidgetOrderbookBuy->item(index.row(),index.column());

            connect(cancelOrderAction, SIGNAL(triggered()), this, SLOT(cancelOrder()));
        }
        //Set contextmenu for Sell Widget
        else if(this->focusWidget()->objectName() == "tableWidgetOrderbookSell" && ui->tableWidgetOrderbookSell->selectedItems().length() == 1)
        {
            QModelIndex index=ui->tableWidgetOrderbookSell->indexAt(pos);

            menu =new QMenu(this);
            cancelOrderAction = new QAction(QIcon(QApplication::applicationDirPath() + "/cancel.png"),"Cancel Order", this);
            menu->addAction(cancelOrderAction);
            menu->popup(ui->tableWidgetOrderbookSell->viewport()->mapToGlobal(pos));

            currentItem = ui->tableWidgetOrderbookSell->item(index.row(),index.column());

            connect(cancelOrderAction, SIGNAL(triggered()), this, SLOT(cancelOrder()));
        }
    }
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
