#include "mex_tradelog.h"
#include "ui_mex_tradelog.h"

MEX_TradeLog::MEX_TradeLog(QList<MEX_Order>& myOrders, QString& userID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MEX_TradeLog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size()); //Set fixed window size
    this->userID = userID;

    //Make table sortable
    connect(ui->tableWidgetMatchedOrders->horizontalHeader(), SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)), this, SLOT(sortMatchedOrdersTable(int,Qt::SortOrder)));

    //Matched Orders in SQL speichern und hier abrufen?
    while (ui->tableWidgetMatchedOrders->rowCount() > 0)
    {
        ui->tableWidgetMatchedOrders->removeRow(0); //Remove all current rows from tablewidget
    }
    QList<MEX_Order>::iterator j;
    for(j = myOrders.begin(); j != myOrders.end(); j++)
    {
        newRow = ui->tableWidgetMatchedOrders->rowCount();

        ui->tableWidgetMatchedOrders->insertRow(newRow);
        ui->tableWidgetMatchedOrders->setItem(newRow, 0,new QTableWidgetItem((*j).getProduct().getSymbol()));
        ui->tableWidgetMatchedOrders->setItem(newRow, 1,new QTableWidgetItem((*j).getProduct().getIndex()));
        ui->tableWidgetMatchedOrders->setItem(newRow, 2,new MEX_TableWidgetItem("0"));
        ui->tableWidgetMatchedOrders->setItem(newRow, 3,new MEX_TableWidgetItem(QString::number((*j).getQuantity())));
        ui->tableWidgetMatchedOrders->setItem(newRow, 4,new MEX_TableWidgetItem(QString::number((*j).getValue())));
        ui->tableWidgetMatchedOrders->setItem(newRow, 5,new QTableWidgetItem((*j).getComment()));
        ui->tableWidgetMatchedOrders->setItem(newRow, 6,new QTableWidgetItem((*j).getTime().toString("hh:mm:ss.zzz")));
    }
    ui->tableWidgetMatchedOrders->sortItems(6);
}

MEX_TradeLog::~MEX_TradeLog()
{
    delete ui;
}

void MEX_TradeLog::sortMatchedOrdersTable(int column, Qt::SortOrder order)
{
ui->tableWidgetMatchedOrders->sortItems(column,order);
}

void MEX_TradeLog::on_bntClear_clicked()
{
    //WEITERARBEITEN: clear löscht auch column titel, nur alle zeilen löschen
    // ui->tableWidgetMatchedOrders->clear();
    while (ui->tableWidgetMatchedOrders->rowCount() > 0)
    {
        ui->tableWidgetMatchedOrders->removeRow(0); //Remove all current rows from tablewidget
    }

}
