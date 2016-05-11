#include "mex_login.h"
#include "ui_mex_login.h"

MEX_Login::MEX_Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MEX_Login)
{
    ui->setupUi(this);

    //Set fixed window size
    this->setFixedSize(this->size());

    //Set SQL DB path
    QString dbPath = QApplication::applicationDirPath() + "/MEX_DB.sqlite";
    db = QSqlDatabase::addDatabase("QSQLITE", "login_connection");
    db.setDatabaseName(dbPath);

    //Connect buttons to RETURN key
    connect(ui->edtUser, SIGNAL(returnPressed()),ui->btnLogIn,SIGNAL(clicked()));
    connect(ui->edtPassword, SIGNAL(returnPressed()),ui->btnLogIn,SIGNAL(clicked()));
    connect(ui->edtConfirmPassword, SIGNAL(returnPressed()),ui->btnSignUp,SIGNAL(clicked()));
}

MEX_Login::~MEX_Login()
{
    delete ui;
}

// Public member functions
/*************************/
inline void MEX_Login::setUserID(QString userID)
{
    this->userID = userID;
}

inline QString MEX_Login::getUserID()
{
    return this->userID;
}

// Private member functions
/*************************/

void MEX_Login::on_btnLogIn_clicked()
{
    logInUser();
}

void MEX_Login::on_btnSignUp_clicked()
{
    registerUser();
}

//Login process
void MEX_Login::logInUser()
{
    //Set proxy if checked and lines not empty
    if(ui->proxy->isChecked())
    {
        if(!ui->edtProxyHost->text().isEmpty())
        {
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(ui->edtProxyHost->text());
            proxy.setPort(ui->spinBoxProxyPort->value());
            if(!(ui->edtProxyUser->text().isEmpty() && ui->edtProxyPassword->text().isEmpty()))
            {
            proxy.setUser(ui->edtProxyUser->text());
            proxy.setPassword(ui->edtProxyPassword->text());
            }
            QNetworkProxy::setApplicationProxy(proxy);
        }
    }

    QString username;
    QString userpass;
    //Get username and password from UI
    username = ui->edtUser->text();
    userpass = ui->edtPassword->text();
    loadUserList();
    if (this->userList.contains(username))
    {
        bool ok = false;
        QString sqlCommand = "SELECT id,pass,usertype  FROM userList WHERE user = '" + username + "' ";
        QSqlQuery query = executeQuery(sqlCommand, ok);
        if (ok)
        {
            //Go to first line
            query.first();
            //Encrypt entered password and compare with encrypted password from DB
            QString cryptPass = encrypt(userpass);
            if (cryptPass == query.record().value(1).toString())
            {
                ui->edtUser->clear();
                ui->edtPassword->clear();
                QString userID = query.record().value(0).toString();
                //Start main application with registered user ID
                openMainWindow(userID);
            } else
            {
                //Incorrect password
                ///übernehmen auf alle anderen msgboxes
                QMessageBox::information(0,"Login failed","Wrong username or password.");
                ui->edtPassword->clear();
            }
        } else
        {
            //Error while executing SQL-Statement
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Could not execute query.");
            messageBox.show();
        }
    } else
    {
        //Incorrect Username
        QMessageBox messageBox;
        messageBox.information(0,"Login failed","Wrong username or password.");
        messageBox.show();
        ui->edtPassword->clear();
    }

}

//Load userList from DB
void MEX_Login::loadUserList()
{
    bool ok = false;
    QString sqlCommand = "SELECT user FROM userList";
    QSqlQuery query(db);
    query = executeQuery(sqlCommand, ok);
    if (ok)
    {
        //Reset userList
        this->userList.clear();
        //Go to first line
        ok = query.first();
        //While there is a next line:
        while (ok)
        {
            //fill list with usernames
            this->userList.append(query.value(0).toString());
            //next line
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

//Registration process
void MEX_Login::registerUser()
{
    //ui->lblSignOutput->clear();
    QString username;
    QString userpass;
    QString confirmpass;
    username = ui->edtUser->text();
    userpass = ui->edtPassword->text();
    confirmpass = ui->edtConfirmPassword->text();
    //Load userList
    loadUserList();
    if(!username.contains(" ") && !userpass.contains(" "))
    {
        if (!this->userList.contains(username))
        {
            if (username.length() > 4 && userpass.length() > 4)
            {
                if(userpass == confirmpass)
                {
                    bool ok = false;
                    QString cryptPass = encrypt(userpass);
                    QString sqlCommand = "INSERT INTO userList (user, pass) VALUES ('" + username + "', '" + cryptPass + "') ";
                    executeQuery(sqlCommand, ok);
                    if (ok)
                    {
                        QMessageBox messageBox;
                        messageBox.information(0,"Registration successful","'" + username + "' registered.");
                        messageBox.show();
                        userList.append(username);
                        ui->edtUser->clear();
                        ui->edtPassword->clear();
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
                    //Passwords do not match
                    QMessageBox messageBox;
                    messageBox.information(0,"Registration failed","Passwords do not match.");
                    messageBox.show();
                    ui->edtPassword->clear();
                    ui->edtConfirmPassword->clear();
                }
            } else
            {
                //Username too short - has to be >= 5 characters
                QMessageBox messageBox;
                messageBox.information(0,"Registration failed","Username and password must be minimum 5 characters.");
                messageBox.show();
            }
        } else
        {
            //Username taken
            QMessageBox messageBox;
            messageBox.information(0,"Registration failed","Username is already taken.");
            messageBox.show();
        }
    } else
    {
        //Spaces in the text
        QMessageBox messageBox;
        messageBox.information(0,"Registration failed","Spaces are not allowed.");
        messageBox.show();
    }
}


//MD5 Hash Encryption
QString MEX_Login::encrypt(QString clearPass)
{
    QByteArray result = hash->hash(clearPass.toUtf8(),QCryptographicHash::Md5);
    return result.toHex();
}



//Successful Login -> open main application with current user ID
void MEX_Login::openMainWindow(QString userID)
{
    //close DB Connection
    closeDB();
    MEX_Main *mainWindow = new MEX_Main(userID);
    mainWindow->show();
    ui->edtUser->setEnabled(false);
    ui->edtPassword->setEnabled(false);
    this->hide();

}

//SQL DATABASE / CONNECTION
QSqlQuery MEX_Login::executeQuery (QString sqlCommand, bool &ok)
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
        ok = query.exec(sqlCommand);
        return query;
    }
}

void MEX_Login::closeDB()
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
