#include "mex_tradelog.h"
#include "ui_mex_tradelog.h"

MEX_TradeLog::MEX_TradeLog(QList<MEX_Order*>& matchedOrders, QString& userID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MEX_TradeLog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size()); //Set fixed window size
    this->userID = userID;


    //Matched Orders in SQL speichern und hier abrufen?
    while (ui->tableWidgetMatchedOrders->rowCount() > 0)
    {
        ui->tableWidgetMatchedOrders->removeRow(0); //Remove all current rows from tablewidget
    }
    QList<MEX_Order*>::iterator j;

    for(j = matchedOrders.begin(); j != matchedOrders.end(); j++)
    {
        if((*j)->getTraderID() == userID){
            newRow = ui->tableWidgetMatchedOrders->rowCount();

            ui->tableWidgetMatchedOrders->insertRow(newRow);
            ui->tableWidgetMatchedOrders->setItem(newRow, 0,new QTableWidgetItem((*j)->getProduct().getSymbol()));
            ui->tableWidgetMatchedOrders->setItem(newRow, 1,new QTableWidgetItem((*j)->getProduct().getIndex()));
            ui->tableWidgetMatchedOrders->setItem(newRow, 2,new QTableWidgetItem("0"));
            ui->tableWidgetMatchedOrders->setItem(newRow, 3,new QTableWidgetItem(QString::number((*j)->getQuantity())));
            ui->tableWidgetMatchedOrders->setItem(newRow, 4,new QTableWidgetItem(QString::number((*j)->getValue())));
            ui->tableWidgetMatchedOrders->setItem(newRow, 5,new QTableWidgetItem((*j)->getComment()));
            ui->tableWidgetMatchedOrders->setItem(newRow, 6,new QTableWidgetItem((*j)->getTime().toString("hh:mm:ss.zzz")));
        }
    }
}

MEX_TradeLog::~MEX_TradeLog()
{
    delete ui;
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
