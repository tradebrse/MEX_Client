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
    this->updated = 0;
    this->gtd = "";
    this->persistent = false; /// Ändern zu konstrutktor wert!!
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
    this->updated = other.updated;
    this->gtd = other.gtd;
    this->persistent = other.persistent;
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
int MEX_Order::getUpdated() const
{
    return this->updated;
}
bool MEX_Order::isTradable() const
{
    return this->tradable;
}

QString MEX_Order::getGTD()
{
    return this->gtd;
}

bool MEX_Order::isPersistent() const
{
    return this->persistent;
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
void MEX_Order::setUpdated(int updated)
{
    this->updated = updated;
}
void MEX_Order::setTradable(bool tradable)
{
    this->tradable = tradable;
}
void MEX_Order::setGTD(QString gtd)
{
    this->gtd = gtd;
}

void MEX_Order::setPersistent(bool persistent)
{
    this->persistent = persistent;
}
