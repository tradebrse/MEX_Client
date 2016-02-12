#include "mex_trader.h"

MEX_Trader::MEX_Trader()
{
}

MEX_Trader::MEX_Trader(const QString traderID, const QString username, QString password, QString permission, int credit)
{
    this->traderID = traderID;
    this->username = username;
    this->password = password;
    this->permission = permission;
    this->credit = credit;
}

MEX_Trader::~MEX_Trader()
{

}

//Getter methods

QString MEX_Trader::getName()
{
    return this->username;
}
QString MEX_Trader::getPassword()
{
    return this->password;
}
QString MEX_Trader::getTraderID()
{
    return this->traderID;
}
QString MEX_Trader::getPermission()
{
    return this->permission;
}
int MEX_Trader::getCredit()
{
    return this->credit;
}

//Setter methods

void MEX_Trader::setName(const QString username)
{
    this->username = username;
}
void MEX_Trader::setPassword(QString password)
{
    this->password = password;
}
void MEX_Trader::setTraderID(QString traderID)
{
    this->traderID = traderID;
}
void MEX_Trader::setPermission(QString permission)
{
    this->permission = permission;
}
void MEX_Trader::setCredit(int credit)
{
    this->credit = credit;
}



