#ifndef MEX_ORDER_H
#define MEX_ORDER_H
#include <QDateTime>
#include <mex_product.h>
#include <iostream>


class MEX_Order
{
public:
    MEX_Order();
    MEX_Order(QString traderID, int orderID, int value, int quantity, QString comment, MEX_Product product, QString ordertype, QDateTime time);
    MEX_Order(const MEX_Order &other);
    ~MEX_Order();

    // Getter methods
    QString getTraderID();
    QString getOrdertype();
    int getOrderID();
    int getValue();
    int getQuantity();
    QDateTime getTime();
    QString getComment();
    MEX_Product getProduct();
    //Setter methods
    void setTraderID(QString);
    void setOrderID(int);
    void setValue(int);
    void setQuantity(int);
    void setComment(QString);
    void setProduct(MEX_Product);
    void setOrdertype(QString);
    void setTime(QDateTime);

private:
    QString traderID;
    int orderID;
    int value;
    int quantity;
    QString comment;
    QDateTime time;
    QString ordertype;
    MEX_Product product;

};
Q_DECLARE_METATYPE(MEX_Order)

#endif // MEX_ORDER_H
