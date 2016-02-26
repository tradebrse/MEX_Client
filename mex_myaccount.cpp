#include "mex_myaccount.h"
#include "ui_mex_myaccount.h"

// Constructor

MEX_MyAccount::MEX_MyAccount(QString userID, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MEX_MyAccount)
{
    ui->setupUi(this);
    this->setFixedSize(this->size()); //Set fixed window size

    //Setup DB
    QString dbPath = QApplication::applicationDirPath() + "/MEX_DB.sqlite";
    db = QSqlDatabase::addDatabase("QSQLITE", "myAccount_connection");
    db.setDatabaseName(dbPath);

    setUserID(userID);

    //Get user information by user ID
    bool ok = false;
    QString sqlCommand = "SELECT user, email, credit, usertype, pass FROM userList WHERE id = '" + userID + "' ";
    QSqlQuery query(db);
    query = executeQuery(sqlCommand, ok);

    if (ok)
    {
        query.first();
        username = query.record().value(0).toString();
        email = query.record().value(1).toString();
        credit = query.record().value(2).toString()+"€";
        usertype = query.record().value(3).toString();
        currentUserPass =  query.record().value(4).toString();
        ui->lblUserOutput->setText(username);
        ui->lblMailOutput->setText(email);
        ui->lblCreditOutput->setText(credit);
        ui->lblUsertypeOutput->setText(usertype);
    } else
    {
        //Error while executing SQL-Statement
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not execute query.");
        messageBox.show();
    }
}

//Deconstructor

MEX_MyAccount::~MEX_MyAccount()
{
    closeDB();
    delete ui;
}

// Public member functions
/*************************/
void MEX_MyAccount::setUserID(QString userID)
{
    this->userID = userID;
}

// Private member functions
/*************************/
void MEX_MyAccount::on_btnChangePW_clicked()
{
    changePassword();
}

void MEX_MyAccount::changePassword(){
    ui->lblInfoOutput->clear();
    QString currentUserPassInput = ui->edtCurrentPassword->text();
    QString newUserpass = ui->edtNewPassword->text();
    QString newUserpassCheck = ui->edtNewPassword_2->text();
    QString cryptCurrentPass = encrypt(currentUserPassInput);
        if (cryptCurrentPass == currentUserPass){
            if(newUserpass == newUserpassCheck)
            {
                if(!newUserpass.contains(" "))
                {
                    if (newUserpass.length() > 4)
                    {
                        QString cryptpass = encrypt(newUserpass);
                        bool ok = false;
                        QString sqlCommand = "UPDATE userList SET pass = '" + cryptpass + "' WHERE user = '" + username + "' ";
                        QSqlQuery query(db);
                        query = executeQuery(sqlCommand, ok);
                        if (ok)
                        {
                            QMessageBox::information(0,"Success","Password has been changed.");
                            ui->edtCurrentPassword->clear();
                            ui->edtNewPassword->clear();
                            ui->edtNewPassword_2->clear();
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
                        messageBox.information(0,"Password change failed","Password must be minimum 5 characters.");
                        messageBox.show();
                    }
                } else
                {
                    QMessageBox messageBox;
                    messageBox.information(0,"Password change failed","Spaces are not allowed.");
                    messageBox.show();
                }
            } else
            {
                QMessageBox messageBox;
                messageBox.information(0,"Password change failed","Your new passwords do not match.");
                messageBox.show();
            }
        }else
        {
            QMessageBox messageBox;
            messageBox.information(0,"Password change failed","Current password is incorrect.");
            messageBox.show();
        }
}

QSqlQuery MEX_MyAccount::executeQuery (QString sqlCommand, bool &ok)
{
    if (!db.open())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No database connection.");
        messageBox.show();
        QSqlQuery emptyQuery;
        return emptyQuery;
    } else {
        QSqlQuery query(db);
        ok = query.exec(sqlCommand);
        return query;
    }
}

void MEX_MyAccount::closeDB()
{
    QString connection;
    connection = db.connectionName();
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(connection);
}

QString MEX_MyAccount::encrypt(QString clearPass)
{
    QByteArray result = hash->hash(clearPass.toUtf8(),QCryptographicHash::Md5);
    return result.toHex();
}
