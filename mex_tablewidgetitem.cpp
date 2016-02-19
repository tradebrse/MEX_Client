#include "mex_tablewidgetitem.h"

MEX_TableWidgetItem::MEX_TableWidgetItem(const QString txt):QTableWidgetItem(txt)
{

}

bool MEX_TableWidgetItem::operator <(const QTableWidgetItem &other) const
{
if( this->text().toInt() < other.text().toInt() )
return true;
else
return false;
}

bool MEX_TableWidgetItem::operator ==(const QTableWidgetItem &other) const
{
if( this->text().toInt() == other.text().toInt() )
return true;
else
return false;
}

bool MEX_TableWidgetItem::operator >(const QTableWidgetItem &other) const
{
if( this->text().toInt() > other.text().toInt() )
return true;
else
return false;
}
