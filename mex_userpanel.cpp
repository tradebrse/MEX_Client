#include "mex_userpanel.h"
#include "ui_mex_userpanel.h"

// Constructor/Deconstructor

MEX_UserPanel::MEX_UserPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MEX_UserPanel)
{
    ui->setupUi(this);
    //Set fixed window size
    this->setFixedSize(this->size());
    //Setup DB
    QString dbPath = QApplication::applicationDirPath() + "/MEX_DB.sqlite";
    db = QSqlDatabase::addDatabase("QSQLITE", "userpanel_connection");
    db.setDatabaseName(dbPath);
}

//Deconstructor

MEX_UserPanel::~MEX_UserPanel()
{
    closeDB();
    delete ui;
}

// Private member functions
/*************************/

void MEX_UserPanel::on_btnDeleteUser_clicked()
{
    deleteUser();
}

void MEX_UserPanel::on_btnShowUsers_clicked()
{
    showUsers();
}

void MEX_UserPanel::on_btnChangePW_clicked()
{
    changePassword();
}

//get username and usertype to delete user
void MEX_UserPanel::deleteUser()
{
    if(ui->lwUsers->selectedItems().count()==0)
    {
        QMessageBox messageBox;
        messageBox.information(0,"Cannot delete user","No user selected.");
        messageBox.show();
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
                        QMessageBox messageBox;
                        messageBox.information(0,"Success","User deleted.");
                        messageBox.show();
                    } else
                    {
                        //Error while executing SQL-Statement
                        QMessageBox messageBox;
                        messageBox.critical(0,"Error","Could not execute query.");
                        messageBox.show();
                    }
                } else
                {
                    QMessageBox messageBox;
                    messageBox.information(0,"Cannot delete user","This user can not be deleted.");
                    messageBox.show();
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
}

void MEX_UserPanel::on_btnShowPW_clicked()
{
    showUserPW();
}

//Get username and read password
void MEX_UserPanel::showUserPW(){

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


void MEX_UserPanel::changePassword() {
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


void MEX_UserPanel::showUsers()
{
    //show username list
    refreshList();
    ui->lwUsers->clear();
    foreach (QString user, this->userList)
    {
        ui->lwUsers->addItem(user);
    }
    ui->btnShowPW->setEnabled(true);
    ui->lwUsers->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->lwUsers->item(0)->setSelected(true);
}

void MEX_UserPanel::refreshList()
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
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not execute query.");
        messageBox.show();
    }
}


QSqlQuery MEX_UserPanel::executeQuery (QString sqlCommand, bool &ok)
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

void MEX_UserPanel::closeDB()
{
    QString connection;
    connection = db.connectionName();
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(connection);
}

QString MEX_UserPanel::encrypt(QString clearPass)
{
    QByteArray result = hash->hash(clearPass.toUtf8(),QCryptographicHash::Md5);
    return result.toHex();
}


