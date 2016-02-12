#include "mex_tcpclientsocket.h"

MEX_TCPClientSocket::MEX_TCPClientSocket(QObject *parent) :
    QObject(parent)
{
}

void MEX_TCPClientSocket::doConnect()
{
    socket = new QTcpSocket(this);

    connect(socket,SIGNAL(disconnected()),this,SIGNAL(clientDisconnected()));

    connect(socket,SIGNAL(readyRead()),this, SLOT(readServerData()));

    socket->connectToHost("127.0.0.1", 1234);
    xmlWriter.setAutoFormatting(true);
    if(socket->waitForConnected(30000)){
        emit clientConnected();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(0,"Connection Error",socket->errorString());
        messageBox.show();
        emit clientDisconnected();
    }
}

void MEX_TCPClientSocket::sendOrder(QString traderID, int value, int quantity, QString comment, QString productsymbol, QString ordertype)
{
    //Write the XML to this socket
    xmlWriter.setDevice(socket);
    //Write start of document and set Order tag
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Order");
    /// AUF SERVER: ZEIT UND ORDER ID NICHT VERGESSEN
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
    //Get the data
    QByteArray data = socket->readAll();
    //Declare and initialize orderbook
    QList<MEX_Order>  currentOrderbook;
    //Convert orderbook data to list of orders
    xmlReader = new QXmlStreamReader(data);
    while(!xmlReader->atEnd())
    {
        //Read line
        xmlReader->readNext();
        if(xmlReader->isStartElement())
        {
            if(xmlReader->name() == "Orderbook")
            {
                readOrders(currentOrderbook);
            }
            else
            {
                ///No relevant data found
                qDebug() << "No Orderbook XML...";
            }
        }
    }
    //Send new orderbook from server to GUI application
    emit serverDataToGUI(currentOrderbook);
}

void MEX_TCPClientSocket::readOrders(QList<MEX_Order>  &orderbook)
{
    //Get first line after orderbook
    xmlReader->readNext();
    //Initialise order variables
    while(!xmlReader->atEnd())
    {
        if(xmlReader->isStartElement())
        {
            //Look for 'Order' entries
            if(xmlReader->name() == "Order")
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
                            order.setValue(xmlReader->readElementText().toInt());
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
                    }
                    //Read next line
                    xmlReader->readNext();
                }
                //Add the new order to the orderbook
                orderbook.append(order);
            }
        }
        else
        {
            xmlReader->readNext();
        }
    }
}
