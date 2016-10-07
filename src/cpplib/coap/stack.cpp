#include "coap.hpp"
#include "stack_p.hpp"
#include "exchange_p.hpp"
#include "message.hpp"
#include "timerqueue.hpp"
#include "contenthandlers.h"

#include <QUdpSocket>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

iotlib::coap::StackPrivate::StackPrivate()
{   
}

iotlib::coap::StackPrivate::~StackPrivate()
{
}

void iotlib::coap::StackPrivate::setup()
{
    Q_Q(iotlib::coap::Stack);
    Coap::addStack(q);

    timerQueue = new TimerQueue(q);
    QObject::connect(timerQueue, SIGNAL(timeout(QByteArray)),
                     q,          SLOT(_q_on_timeout(QByteArray)));

    // register built in content handlers
//    Coap::addUnpacker((quint16)iotlib::coap::Message::ContentFormat::AppJson,
//                      &CoapContentHandlers::unpackJSONContent);
//    Coap::addUnpacker((quint16)77, // TODO msgpack content format to config
//                      &CoapContentHandlers::unpackMsgPackContent);
}

void iotlib::coap::StackPrivate::tx(Exchange *fromExchange, iotlib::coap::Message &message)
{
    if (message.isRequest())
        txRequest(fromExchange, message);
    else if (message.isResponse())
        txResponse(fromExchange, message);
    else
        txEmpty(fromExchange, message);
}

void iotlib::coap::StackPrivate::txRequest(Exchange *fromExchange, iotlib::coap::Message &request)
{
    removeExchange(fromExchange); // remove previous data

    if (request.messageId() == 0)
        request.setMessageId(currentMid++);

//    MidAddressPortKey midKey(request.messageId());
//    exchangeByMid.insert(midKey, fromExchange);

    if (request.token().isEmpty()) {
        QByteArray token = generateUniqueToken();
        request.setToken(token);
        exchangeByToken.insert(token, fromExchange);
    } else {
        /// TODO ongoing exchanges may reuse token, don't show warning in this case
        if (exchangeByToken.contains(request.token()))
            qWarning() << "Token reusing" << request.token().toHex();
    }

    if (request.type() == iotlib::coap::Message::Type::Confirmable) {// schedule retransmission
        fromExchange->d_ptr->retransmissionCount = 0;
        timerQueue->addTimer(2000, request.token());
    }

    sendMessage(request);
}

QByteArray iotlib::coap::StackPrivate::generateUniqueToken()
{
    QByteArray token;
    // TODO token size from config
    token.resize(2);
    do {
        quint8 *d = (quint8 *)token.data();
        for (int i = 0; i < token.size(); ++i)
            d[i] = rand() % 255;
    } while (exchangeByToken.contains(token));
    return token;
}

void iotlib::coap::StackPrivate::_q_on_timeout(const QByteArray &key)
{
    Exchange *exchange = exchangeByToken.value(key, 0);
    if (!exchange)
        return;
    if (++exchange->d_ptr->retransmissionCount == 4) { // give up
        exchange->handleError();
    } else {
        sendMessage(exchange->d_ptr->message);
        timerQueue->addTimer(2000, key);
    }
}

void iotlib::coap::StackPrivate::txResponse(Exchange *fromExchange, iotlib::coap::Message &response)
{

}

void iotlib::coap::StackPrivate::txEmpty(Exchange *fromExchange, iotlib::coap::Message &empty)
{
    Q_UNUSED(fromExchange);
    sendMessage(empty);
}

void iotlib::coap::StackPrivate::rx(iotlib::coap::Message &message)
{
    if (message.isRequest())
        rxRequest(message);
    else if (message.isResponse())
        rxResponse(message);
    else
        rxEmpty(message);
}

void iotlib::coap::StackPrivate::rxRequest(iotlib::coap::Message &request)
{

}

void iotlib::coap::StackPrivate::rxResponse(iotlib::coap::Message &response)
{
    if (response.type() == iotlib::coap::Message::Type::Reset) {
        qDebug() << "Ignoring response with Type::Reset";
        return;
    }
    Exchange *exchange = exchangeByToken.value(response.token(), 0);
    if (exchange) {
        qDebug() << "found exchange" << exchange;
        timerQueue->removeTimer(response.token());
        exchange->handle(response);
    } else {
        qDebug() << "Strange or after observe response received, RST it";
        iotlib::coap::Message rst;
        rst.setAddress(response.address());
        rst.setType(iotlib::coap::Message::Type::Reset);
        rst.setToken(response.token());
        rst.setMessageId(response.messageId());
        tx(0, rst);
    }
}

void iotlib::coap::StackPrivate::rxEmpty(iotlib::coap::Message &empty)
{

}

void iotlib::coap::StackPrivate::removeExchange(Exchange *exchange)
{
    QByteArray token = exchangeByToken.key(exchange);
    if (token.isEmpty())
        return;
    qDebug() << "Removing exchange with token:" << token.toHex();
    exchangeByToken.remove(token);
    timerQueue->removeTimer(token);
}

void iotlib::coap::StackPrivate::_q_on_message_received(Message &message)
{

}

iotlib::coap::Stack::Stack(QObject *parent) :
    QObject(parent), d_ptr(new iotlib::coap::StackPrivate)
{
    Q_D(iotlib::coap::Stack);
    d->q_ptr = this;
    d->setup();
}

iotlib::coap::Stack::Stack(iotlib::coap::StackPrivate &dd, QObject *parent) :
    QObject(parent), d_ptr(&dd)
{
    Q_D(iotlib::coap::Stack);
    d->q_ptr = this;
    d->setup();
}

iotlib::coap::Stack::~Stack()
{
    if (d_ptr) {
        delete d_ptr;
        d_ptr = 0;
    }
}

#include "moc_stack.cpp"
