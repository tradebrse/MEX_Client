#ifndef MEX_PRODUCT_H
#include <QString>
#include <iostream>

class MEX_Product
{
public:
    MEX_Product();
    ~MEX_Product();

    QString getName();
    QString getSymbol();
    QString getIndex();

    void setName(QString);
    void setSymbol(QString);
    void setIndex(QString);

    bool operator==(const MEX_Product &product) const;

private:
    QString name;
    QString symbol;
    QString index;
};

#endif // MEX_PRODUCT_H
