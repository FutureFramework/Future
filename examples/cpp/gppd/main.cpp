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

    e->setUrl(QUrl("coap://coap.me2/hello"));
    e->get();

    QObject::connect(e, &CoapExchange::statusChanged, [=](){ qDebug() << e->status(); });


    return app.exec();
}
