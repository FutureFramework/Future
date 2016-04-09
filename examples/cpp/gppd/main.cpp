#include <QCoreApplication>
#include "coap/coapendpoint.hpp"
#include "coap/coapexchange.hpp"
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    CoapEndpoint endpoint;
    endpoint.bind(QHostAddress::Any, 5686);



    CoapExchange *e = new CoapExchange;
    qDebug() << e->status();

    QObject::connect(e, &CoapExchange::statusChanged, [=](){ qDebug() << e->status(); });
    QObject::connect(e, &CoapExchange::completed, [=](){
       qDebug() << "Unpacked payload:" << e->content();
    });


    e->setUrl(QUrl("coap://127.0.0.1/hello"));
    e->get();



    return app.exec();
}
