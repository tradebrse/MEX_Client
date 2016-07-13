#include "mex_adminpanel.h"
#include "ui_mex_adminpanel.h"
// Constructor/Deconstructor

MEX_AdminPanel::MEX_AdminPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MEX_AdminPanel)
{  
    ui->setupUi(this);
    //Set fixed window size
    this->setFixedSize(this->size());
    //Setup DB
    QString dbPath = QApplication::applicationDirPath() + "/MEX_DB.sqlite";
    db = QSqlDatabase::addDatabase("QSQLITE", "AdminPanel_connection");
    db.setDatabaseName(dbPath);

    loadProductList();

    QStringList distinctProductIndexList;
    for(int i = 0; i < productIndexList.length(); i++)
    {
        if(!distinctProductIndexList.contains(productIndexList.value(i)))
        {
            distinctProductIndexList.append(productIndexList.value(i));
            ui->cBoxIndices->addItem(productIndexList.value(i));
        }
    }
}

//Deconstructor

MEX_AdminPanel::~MEX_AdminPanel()
{
    closeDB();
    delete ui;
}

// Private member functions
/*************************/

void MEX_AdminPanel::on_btnDeleteUser_clicked()
{
    deleteUser();
}

void MEX_AdminPanel::on_btnShowUsers_clicked()
{
    showUsers();
}

void MEX_AdminPanel::on_btnChangePW_clicked()
{
    changePassword();
}

//get username and usertype to delete user
void MEX_AdminPanel::deleteUser()
{
    if(ui->lwUsers->selectedItems().count()==0)
    {
        QMessageBox::information(0,"Cannot delete user","No user selected.");
    }
    else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this user?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            QString user = ui->lwUsers->selectedItems().first()->text();

            bool ok = false;
            QString sqlCommand = "SELECT usertype FROM userList WHERE user = '" + user + "' ";
            QSqlQuery query(db);
            query = executeQuery(sqlCommand, ok);

            if (ok)
            {
                query.first();
                if (query.record().value(0).toString() == "client")
                {
                    bool ok = false;
                    QString sqlCommand = "DELETE FROM userList WHERE user = '" + user + "' ";
                    executeQuery(sqlCommand, ok);
                    if (ok)
                    {
                        showUsers();
                        QMessageBox::information(0,"Success","User deleted.");
                    } else
                    {
                        //Error while executing SQL-Statement
                        QMessageBox::critical(0,"Error","Could not execute query.");
                    }
                } else
                {
                    QMessageBox::information(0,"Cannot delete user","This user can not be deleted.");
                }
            } else
            {
                //Error while executing SQL-Statement
                QMessageBox::critical(0,"Error","Could not execute query.");
            }
        }
    }
}

void MEX_AdminPanel::on_btnShowPW_clicked()
{
    showUserPW();
}

//Get username and read password
void MEX_AdminPanel::showUserPW(){

    if(ui->lwUsers->selectedItems().count() == 0)
    {
        QMessageBox messageBox;
        messageBox.information(0,"Cannot show password","No User selected.");
        messageBox.show();
    } else
    {
        QString user = ui->lwUsers->selectedItems().first()->text();
        bool ok = false;
        QString sqlCommand = "SELECT pass FROM userList WHERE user = '" + user + "' ";
        QSqlQuery query(db);
        query = executeQuery(sqlCommand, ok);
        if (ok)
        {
            query.first();
            if (query.record().value(0).toString() != "")
            {
                ui->lwUsers->selectedItems().first()->setText(user + "  :  " + query.record().value(0).toString());
            }
        } else
        {
            //Error while executing SQL-Statement
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Could not execute query.");
            messageBox.show();
        }
    }
}


void MEX_AdminPanel::changePassword() {
    if(ui->lwUsers->selectedItems().count() == 0)
    {
        QMessageBox messageBox;
        messageBox.information(0,"Could not change password","No User selected.");
        messageBox.show();
    } else
    {
        QString username = ui->lwUsers->selectedItems().first()->text();
        QString userpass = ui->edtNewPW->text();

        if(!userpass.contains(" "))
        {
            if (userpass.length() > 4)
            {
                QString cryptpass = encrypt(userpass);
                bool ok = false;
                QString sqlCommand = "UPDATE userList SET pass = '" + cryptpass + "' WHERE user = '" + username + "' ";
                QSqlQuery query(db);
                query = executeQuery(sqlCommand, ok);
                if (ok)
                {
                    ui->edtNewPW->clear();
                    QMessageBox messageBox;
                    messageBox.information(0,"Success","Password has been changed.");
                    messageBox.show();
                } else
                {
                    //Error while executing SQL-Statement
                    QMessageBox messageBox;
                    messageBox.critical(0,"Error","Could not execute query.");
                    messageBox.show();
                }
            }
            else
            {
                QMessageBox messageBox;
                messageBox.information(0,"Could not change password","Password must be minimum 5 characters.");
                messageBox.show();
            }
        } else
        {
            QMessageBox messageBox;
            messageBox.information(0,"Could not change password","Spaces are not allowed.");
            messageBox.show();
        }
    }
}


void MEX_AdminPanel::showUsers()
{
    //show username list
    refreshUserList();
    ui->lwUsers->clear();
    foreach (QString user, this->userList)
    {
        ui->lwUsers->addItem(user);
    }
    ui->lwUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->lwUsers->item(0)->setSelected(true);
}

void MEX_AdminPanel::refreshUserList()
{
    bool ok = false;
    QString sqlCommand = "SELECT user FROM userList";
    QSqlQuery query(db);
    query = executeQuery(sqlCommand, ok);
    if (ok)
    {
        this->userList.clear();
        //Go to first line
        ok = query.first();
        //While there is a next line
        while (ok)
        {
            //Fill list with usernames
            this->userList.append(query.value(0).toString());
            //Get next line
            ok = query.next();
        }
    } else
    {
        //Error while executing SQL-Statement
        QMessageBox::critical(0,"Error","Could not execute query.");
    }
}


void MEX_AdminPanel::on_btnShowProducts_clicked()
{
    showProducts();
}

void MEX_AdminPanel::on_btnAddProduct_clicked()
{
    addProduct();
}

void MEX_AdminPanel::on_btnDeleteProduct_clicked()
{
    deleteProduct();
}

void MEX_AdminPanel::loadProductList()
{
    productIndexList.clear();
    productNameList.clear();
    productSymbolList.clear();
    bool ok = false;
    QString sqlCommand = "SELECT symbol,name,indexName FROM productList ORDER BY indexName ASC, name ASC";
    QSqlQuery query(db);
    query = executeQuery(sqlCommand, ok);
    if (ok)
    {
        this->userList.clear();
        //Go to first line
        ok = query.first();
        //While there is a next line
        while (ok)
        {
            //Fill list with product information
            this->productSymbolList.append(query.value(0).toString());
            this->productNameList.append(query.value(1).toString());
            this->productIndexList.append(query.value(2).toString());
            //Get next line
            ok = query.next();
        }
    } else
    {
        //Error while executing SQL-Statement
        QMessageBox::critical(0,"Error","Could not execute query.");
    }
}

void MEX_AdminPanel::showProducts()
{
    //Show product list
    loadProductList();
    ui->lwProducts->clear();
    for(int i = 0; i < productNameList.length(); i++)
    {
        ui->lwProducts->addItem(productIndexList.value(i) + " | " + productNameList.value(i));
    }
    ui->lwProducts->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->lwProducts->item(0)->setSelected(true);
}


void MEX_AdminPanel::deleteProduct()
{
    if(ui->lwProducts->selectedItems().count()==0)
    {
        QMessageBox::information(0,"Cannot delete product","No product selected.");
    }
    else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this product?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            QString productSymbol = productSymbolList.at(ui->lwProducts->currentRow());
            bool ok = false;
            QString sqlCommand = "DELETE FROM productList WHERE symbol = '" + productSymbol + "' ";
            executeQuery(sqlCommand, ok);
            if (ok)
            {
                showProducts();
                QMessageBox::information(0,"Success","Product deleted.");
            } else
            {
                //Error while executing SQL-Statement
                QMessageBox::critical(0,"Error","Could not execute query.");
            }
        }
    }
}

void MEX_AdminPanel::addProduct()
{
    QString index;
    QString symbol;
    QString name;
    index = ui->cBoxIndices->currentText();
    symbol = ui->edtProductSymbol->text();
    name = ui->edtProductName->text();

    bool ok = false;
    QString sqlCommand = "INSERT INTO productList (indexName, symbol, name) VALUES ('" + index + "', '" + symbol + "', '" + name + "') ";
    executeQuery(sqlCommand, ok);
    if (ok)
    {
        QMessageBox::information(0,"Success","Product added.");
        ui->edtProductSymbol->clear();
        ui->edtProductName->clear();
        showProducts();
    } else
    {
        //Error while executing SQL-Statement
        QMessageBox::critical(0,"Error","Could not execute query.");
    }
}

QString MEX_AdminPanel::encrypt(QString clearPass)
{
    QByteArray result = QCryptographicHash::hash(clearPass.toUtf8(),QCryptographicHash::Md5);
    return result.toHex();
}

QSqlQuery MEX_AdminPanel::executeQuery (QString sqlCommand, bool &ok)
{
    if (!db.open())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No database connection.");
        messageBox.show();
        QSqlQuery emptyQuery;
        return emptyQuery;
    } else
    {
        QSqlQuery query(db);
        //QSqlQuery query
        ok = query.exec(sqlCommand);
        return query;
    }
}

void MEX_AdminPanel::closeDB()
{
    QString connection;
    connection = db.connectionName();
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(connection);
}

void MEX_AdminPanel::on_btnSOD_clicked()
{  
    if(!open)
    {
        emit changeExchangeStatus(QString("SOD").toUtf8());
        open = true;
    }
}


void MEX_AdminPanel::on_btnEOD_clicked()
{
    if(open)
    {
        emit changeExchangeStatus(QString("EOD").toUtf8());
        open = false;
    }
}

void MEX_AdminPanel::setExchangeStatus(bool open)
{
    this->open = open;
}
