#ifndef COAP_EXCHANGE_P_H
#define COAP_EXCHANGE_P_H

#include "exchange.hpp"

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

    CoapExchange *q_ptr;

    CoapEndpoint *endpoint;

    CoapExchange::Status status;
    void setStatus(CoapExchange::Status status);

    void _q_looked_up(const QHostInfo &info);

    quint8 retransmissionCount;
    CoapMessage message;
    QUrl url;
    QByteArray payload;

    bool sendAfterLookup;
    bool deleteAfterComplete;
    bool observe;

    bool isReady();
};

#endif // COAP_EXCHANGE_P_H

