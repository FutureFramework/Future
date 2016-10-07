#ifndef COAP_ENDPOINTBASE_H
#define COAP_ENDPOINTBASE_H

#include "message.hpp"

namespace iotlib {
namespace coap {

class EndpointBase : public QObject
{
    Q_OBJECT
public:
    EndpointBase(QObject *parent = 0) : QObject(parent) { }
    virtual ~EndpointBase() {}

public slots:
    virtual void send(const Message &coapMessage) = 0;

signals:
    void received(Message &coapMessage);
};

} // coap
} // iotlib

#endif // COAP_ENDPOINTBASE_H
