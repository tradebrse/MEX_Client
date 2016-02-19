#ifndef MEX_TABLEWIDGETITEM_H
#define MEX_TABLEWIDGETITEM_H
#include <QString>
#include <QTableWidgetItem>


class MEX_TableWidgetItem : public QTableWidgetItem
{
public:
    MEX_TableWidgetItem(const QString txt);


    bool operator <(const QTableWidgetItem &other) const;
    bool operator ==(const QTableWidgetItem &other) const;
    bool operator >(const QTableWidgetItem &other) const;
};

#endif // MEX_TABLEWIDGETITEM_H
