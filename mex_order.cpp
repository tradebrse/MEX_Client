#include "mex_order.h"

MEX_Order::MEX_Order()
{
}

MEX_Order::MEX_Order(QString traderID, int orderID, double value, int quantity, QString comment, MEX_Product product, QString ordertype, QDateTime time)
{
    this->traderID = traderID;
    this->orderID = orderID;
    this->value = value;
    this->quantity = quantity;
    this->comment = comment;
    this->product = product;
    this->ordertype = ordertype;
    this->time = time;
    ///time = QDateTime::currentDateTime();
}

//Copy constructor
MEX_Order::MEX_Order(const MEX_Order &other)
{
    this->traderID = other.traderID;
    this->orderID = other.orderID;
    this->value = other.value;
    this->quantity = other.quantity;
    this->comment = other.comment;
    this->product = other.product;
    this->ordertype = other.ordertype;
    this->time = other.time;
}

MEX_Order::~MEX_Order()
{

}

//Getter methods
QString MEX_Order::getTraderID()
{
    return this->traderID;
}
QString MEX_Order::getOrdertype()
{
    return this->ordertype;
}
int MEX_Order::getOrderID()
{
    return this->orderID;
}
double MEX_Order::getValue()
{
    return this->value;
}
int MEX_Order::getQuantity()
{
    return this->quantity;
}
QString MEX_Order::getComment()
{
    return this->comment;
}
QDateTime MEX_Order::getTime()
{
    return this->time;
}
MEX_Product MEX_Order::getProduct()
{
    return this->product;
}

//Setter methods
void MEX_Order::setTraderID(QString traderID)
{
   this->traderID = traderID;
}
void MEX_Order::setOrderID(int orderID)
{
    this->orderID = orderID;
}
void MEX_Order::setValue(double value)
{
    this->value = value;
}
void MEX_Order::setQuantity(int quantity)
{
    this->quantity = quantity;
}
void MEX_Order::setComment(QString comment)
{
    this->comment = comment;
}
void MEX_Order::setProduct(MEX_Product product)
{
    this->product = product;
}
void MEX_Order::setOrdertype(QString ordertype)
{
    this->ordertype = ordertype;
}
void MEX_Order::setTime(QDateTime time)
{
    this->time = time;
}
