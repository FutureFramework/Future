#include <QCoreApplication>
#include "coap/stack.hpp"
#include <QDebug>

using namespace iotlib::coap;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    Stack endpoint;


//    CoapExchange *e = new CoapExchange;
//    qDebug() << e->status();

//    QObject::connect(e, &CoapExchange::statusChanged, [=](){ qDebug() << e->status(); });
//    QObject::connect(e, &CoapExchange::completed, [=](){
//       qDebug() << "Unpacked payload:" << e->content();
//    });


//    e->setUrl(QUrl("coap://127.0.0.1/hello"));
//    e->get();



    return app.exec();
}
