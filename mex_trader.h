#ifndef MEX_TRADER_H
#define MEX_TRADER_H
#include <QString>

class MEX_Trader
{
public:
    MEX_Trader();
         //Overloaded constructor that creates a trader object with all information given
    MEX_Trader( const QString traderID, const QString username, QString password, QString permission, int credit );
    ~MEX_Trader();


    QString getName();
    void setName(const QString);
    QString getPassword();
    void setPassword(QString);
    QString getTraderID();
    void setTraderID(QString);
    QString getPermission();
    void setPermission(QString);
    int getCredit();
    void setCredit(int);

private:
    QString username;
    QString password;
    QString traderID;
    QString permission;
    int credit;
};

#endif // MEX_TRADER_H
