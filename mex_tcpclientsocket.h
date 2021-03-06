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
    explicit MEX_TCPClientSocket(QString traderID, QObject *parent = 0);

    ~MEX_TCPClientSocket();

    void doConnect();

    void sendOrder(QString traderID, double value, int quantity, QString comment, QString productsymbol, QString ordertype, bool persistent);

    void sendOrder(QString traderID, double value, int quantity, QString comment, QString productsymbol, QString ordertype, QString gtd, bool persistent);

    void requestOrderbook();

    QString readMessage();

    void doDisconnect();
signals:
    void clientConnected();
    void clientDisconnected();
    void serverDataToGUI(QList<MEX_Order> currentOrderbook, QList<MEX_Order> matchedOrders);
    void exchangeStatusChanged(bool);
public slots:
    void readServerData();
    void writeRawData(QByteArray);
private slots:
    void readOrders(QList<MEX_Order>& orderbook);
private:
    QTcpSocket* socket;
    QXmlStreamWriter* xmlWriter;
    QXmlStreamReader* xmlReader;
    MEX_Order order;
    QString traderID;
};


#endif // MEX_TCPCLIENTSOCKET_H
