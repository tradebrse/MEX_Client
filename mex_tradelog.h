#ifndef MEX_TRADELOG_H
#define MEX_TRADELOG_H

#include <QDialog>
#include <mex_order.h>

namespace Ui {
class MEX_TradeLog;
}

class MEX_TradeLog : public QDialog
{
    Q_OBJECT

public:
    explicit MEX_TradeLog(QList<MEX_Order*>&, QString&, QWidget *parent = 0);

    ~MEX_TradeLog();

private slots:
    void on_bntClear_clicked();

private:
    Ui::MEX_TradeLog *ui;
    int newRow;
    QString userID;
};

#endif // MEX_TRADELOG_H
