#ifndef COAP_EXCHANGE_P_H
#define COAP_EXCHANGE_P_H

#include "exchange.hpp"

#include <functional>
#include <QJSValue>
#include <QHostInfo>
#include <QUrl>

namespace iotlib {
namespace coap {

class Stack;
class ExchangePrivate
{
    Q_DECLARE_PUBLIC(Exchange)
public:
    ExchangePrivate();
    virtual ~ExchangePrivate();

    Exchange *q_ptr;

    Stack *stack;

    Exchange::Status status;
    void setStatus(Exchange::Status status);

    void _q_looked_up(const QHostInfo &info);

    quint8 retransmissionCount;
    Message message;
    QUrl url;
    QByteArray payload;

    bool sendAfterLookup;
    bool deleteAfterComplete;
    bool observe;

    bool isReady();
};

} // coap
} // iotlib

#endif // COAP_EXCHANGE_P_H

