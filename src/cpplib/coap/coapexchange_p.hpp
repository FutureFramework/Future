#ifndef COAP_EXCHANGE_P_H
#define COAP_EXCHANGE_P_H

#include "coapexchange.hpp"

#include <functional>
#include <QJSValue>
#include <QHostInfo>
#include <QUrl>

class CoapEndpoint;
class CoapExchangePrivate
{
    Q_DECLARE_PUBLIC(CoapExchange)
public:
    CoapExchangePrivate();
    virtual ~CoapExchangePrivate();

    CoapEndpoint *endpoint;
    CoapExchange *q_ptr;

    CoapExchange::Status status;
    void setStatus(CoapExchange::Status status);

    void _q_looked_up(const QHostInfo &info);

    QJSValue jsCompleted;
    QJSValue jsTimeout;

    quint8 retransmissionCount;
    CoapMessage lastRequest;
    QUrl url;

    bool sendAfterLookup;
    bool deleteAfterComplete;
    bool observe;

    bool isReady();
};

#endif // COAP_EXCHANGE_P_H

