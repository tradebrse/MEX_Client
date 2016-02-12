#ifndef MEX_TCPCLIENTSOCKET_H
#define MEX_TCPCLIENTSOCKET_H

#include <QTcpSocket>
#include <QObject>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <mex_order.h>
#include <QMessageBox>
#include <QDebug>


class MEX_TCPClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit MEX_TCPClientSocket(QObject *parent = 0);

    void doConnect();

    void sendOrder(QString traderID, int value, int quantity, QString comment, QString productsymbol, QString ordertype);

    void requestOrderbook();

    QString readMessage();

    void doDisconnect();
signals:
    void clientConnected();
    void clientDisconnected();
    void serverDataToGUI(QList<MEX_Order> currentOrderbook);
public slots:
    void readServerData();
private slots:
    void readOrders(QList<MEX_Order>& orderbook);
private:
    QTcpSocket *socket;
    QXmlStreamWriter xmlWriter;
    QXmlStreamReader* xmlReader;
    MEX_Order order;
};


#endif // MEX_TCPCLIENTSOCKET_H
