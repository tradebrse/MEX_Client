#ifndef MEX_ORDER_H
#define MEX_ORDER_H
#include <QDateTime>
#include <mex_product.h>
#include <iostream>

class MEX_Order
{
public:
    MEX_Order();
    MEX_Order(QString traderID, int orderID, double value, int quantity, QString comment, MEX_Product product, QString ordertype, QDateTime time);
    MEX_Order(const MEX_Order &other);
    ~MEX_Order();

    // Getter methods
    QString getTraderID();
    QString getOrdertype();
    int getOrderID();
    double getValue();
    int getQuantity();
    QDateTime getTime();
    QString getComment();
    MEX_Product getProduct();
    int getUpdated() const;
    bool isTradable() const;
    QString getGTD();
    bool isPersistent() const;

    //Setter methods
    void setTraderID(QString);
    void setOrderID(int);
    void setValue(double);
    void setQuantity(int);
    void setComment(QString);
    void setProduct(MEX_Product);
    void setOrdertype(QString);
    void setTime(QDateTime);
    void setUpdated(int);
    void setTradable(bool);
    void setGTD(QString gtd);
    void setPersistent(bool persistent);

private:
    QString traderID;
    int orderID;
    double value;
    int quantity;
    QString comment;
    QDateTime time;
    QString ordertype;
    MEX_Product product;
    int updated;
    bool tradable;
    QString gtd;
    bool persistent;
};
#endif //MEX_ORDER_H
