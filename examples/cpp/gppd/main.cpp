#include <QCoreApplication>
#include "coap/coapendpoint.hpp"
#include "coap/coapexchange.hpp"
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QUrl url("coap://{some}/av");
    qDebug() << url;
    qDebug() << url.scheme();
    qDebug() << url.host();
    qDebug() << url.path();
    qDebug() << url.port();
    qDebug() << url.query();
    qDebug() << QUrl::idnWhitelist();

    return 0;

//    CoapEndpoint endpoint;
//    endpoint.bind(QHostAddress::Any, 5686);

//    CoapExchange *e = new CoapExchange;
//    qDebug() << e->status();

//    e->setUri(CoapUri("coap://coap.me"));
//    e->get();

//    QObject::connect(e, &CoapExchange::statusChanged, [=](){ qDebug() << e->status(); });


    return app.exec();
}
