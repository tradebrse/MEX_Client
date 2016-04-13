#include "mex_tcpclientsocket.h"
#include <qhostaddress.h>
MEX_TCPClientSocket::MEX_TCPClientSocket(QString traderID, QObject *parent) :
    QObject(parent)
{
    this->traderID = traderID;
}

void MEX_TCPClientSocket::doConnect()
{
    socket = new QTcpSocket(this);

    connect(socket,SIGNAL(disconnected()),this,SIGNAL(clientDisconnected()));

    connect(socket,SIGNAL(readyRead()),this, SLOT(readServerData()));

    socket->connectToHost("127.0.0.1", 1234);

    xmlWriter.setAutoFormatting(true);
    if(socket->waitForConnected(10000)){
        emit clientConnected();
        //Send empty order to give traderID information to server
        sendOrder(this->traderID,0,0,"","","");
    }
    else
    {
        QMessageBox::warning(0,"Connection Error",socket->errorString());
        emit clientDisconnected();
    }
}

void MEX_TCPClientSocket::sendOrder(QString traderID, double value, int quantity, QString comment, QString productsymbol, QString ordertype)
{
    //Write the XML to this socket
    xmlWriter.setDevice(socket);
    //Write start of document and set Order tag
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Order");
    //Write trader ID
    xmlWriter.writeStartElement("Trader_ID");
    xmlWriter.writeCharacters(traderID);
    xmlWriter.writeEndElement();
    //Write value data
    xmlWriter.writeStartElement("Value");
    xmlWriter.writeCharacters(QString::number(value));
    xmlWriter.writeEndElement();
    //Write quantity data
    xmlWriter.writeStartElement("Quantity");
    xmlWriter.writeCharacters(QString::number(quantity));
    xmlWriter.writeEndElement();
    //Write comment data
    xmlWriter.writeStartElement("Comment");
    xmlWriter.writeCharacters(comment);
    xmlWriter.writeEndElement();
    //Write product data
    xmlWriter.writeStartElement("Product");
    xmlWriter.writeCharacters(productsymbol);
    xmlWriter.writeEndElement();
    //Write ordertype data
    xmlWriter.writeStartElement("Order_Type");
    xmlWriter.writeCharacters(ordertype);
    xmlWriter.writeEndElement();
    //Close tag Order
    xmlWriter.writeEndElement();
    //Close document
    xmlWriter.writeEndDocument();

    //Wait till order was sent
    socket->waitForBytesWritten(-1); //Eventuell ander Zeit setzen - If msecs is -1, this function will not time out.
}

void MEX_TCPClientSocket::requestOrderbook()
{
    //Write the XML to this socket
    xmlWriter.setDevice(socket);
    //Write start of document and set Order tag
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Orderbook");
    //Close tag Order
    xmlWriter.writeEndElement();
    //Close document
    xmlWriter.writeEndDocument();
    socket->waitForBytesWritten(-1); //Eventuell ander Zeit setzen - If msecs is -1, this function will not time out.
}


void MEX_TCPClientSocket::doDisconnect()
{
    //Close the connection
    socket->close();
}

void MEX_TCPClientSocket::readServerData()
{
    //Get the data and make sure all bytes are read
    QByteArray data;
    data = socket->readAll();
    while(socket->waitForReadyRead(50))
    {
        while(socket->bytesAvailable() > 0)
        {
            data.append(socket->readAll());
            socket->flush();
        }
    }
    //Declare and initialize matched orders list
    QList<MEX_Order>  matchedOrders;
    //Declare and initialize order book
    QList<MEX_Order>  currentOrderbook;
    //Convert orderbook data to list of orders
    xmlReader = new QXmlStreamReader(data);
    while(!xmlReader->atEnd())
    {
        //Read line
        xmlReader->readNext();
        if(xmlReader->isStartElement())
        {
            if(xmlReader->name() == "Orderlists")
            {
                while(!xmlReader->atEnd())
                {
                    xmlReader->readNext();
                    if(xmlReader->isStartElement())
                    {
                        if(xmlReader->name() == "Orderbook")
                        {
                            readOrders(currentOrderbook);
                        }
                        else if(xmlReader->name() == "MatchedOrders")
                        {
                            readOrders(matchedOrders);
                        }
                    }
                }
            }
            else
            {
                //No relevant data found
                QMessageBox::warning(0,"Unvalid data","Could not receive valid data from the server.");
            }
        }
    }
    //Send new orderbook from server to GUI application
    emit serverDataToGUI(currentOrderbook, matchedOrders);
}

void MEX_TCPClientSocket::readOrders(QList<MEX_Order>  &orderbook)
{
    //Get first line after order list description - e.g. Orderbook / MatchedOrders
    xmlReader->readNext();
    //Initialize order variables
    while(!xmlReader->atEnd() && xmlReader->name() != "Separator")
    {
        //Look for 'Order' entries
        if(xmlReader->isStartElement() && xmlReader->name() == "Order")
        {
            //Create order object
            MEX_Order order;
            xmlReader->readNext();
            while(xmlReader->name() != "Order")
            {
                if (xmlReader->isStartElement())
                {
                    //Set attributes for order
                    if (xmlReader->name() == "Trader_ID")
                    {
                        order.setTraderID(xmlReader->readElementText());
                    }
                    else if (xmlReader->name() == "Order_ID")
                    {
                        order.setOrderID(xmlReader->readElementText().toInt());
                    }
                    else if (xmlReader->name() == "Value")
                    {
                        order.setValue(xmlReader->readElementText().toDouble());
                    }
                    else if (xmlReader->name() == "Quantity")
                    {
                        order.setQuantity(xmlReader->readElementText().toInt());
                    }
                    else if (xmlReader->name() == "Comment")
                    {
                        order.setComment(xmlReader->readElementText());
                    }
                    else if (xmlReader->name() == "Product")
                    {
                        MEX_Product product;
                        product.setSymbol(xmlReader->readElementText());
                        product.setIndex("");
                        product.setName("");
                        order.setProduct(product);
                    }
                    else if (xmlReader->name() == "Order_Type")
                    {
                        order.setOrdertype(xmlReader->readElementText());
                    }
                    else if (xmlReader->name() == "Time")
                    {
                        QDateTime time = QDateTime::fromString(xmlReader->readElementText(),"hh:mm:ss.zzz");
                        order.setTime(time);
                    }
                    else if (xmlReader->name() == "Update")
                    {
                        order.setUpdated(xmlReader->readElementText().toInt());
                    }
                }
                //Read next line
                xmlReader->readNext();
            }
            //Add the new order to the orderbook
            orderbook.append(order);
        }
        else
        {
            xmlReader->readNext();
        }
    }
}
