#include "coap.hpp"
#include "coapendpoint_p.hpp"
#include "coapexchange_p.hpp"
#include "coapmessage.hpp"
#include "timerqueue.hpp"
#include "coapcontenthandlers.h"

#include <QUdpSocket>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

CoapEndpointPrivate::CoapEndpointPrivate() :
    interface(QHostAddress::Null), port(0)
{   
}

CoapEndpointPrivate::~CoapEndpointPrivate()
{
}

void CoapEndpointPrivate::setup()
{
    Q_Q(CoapEndpoint);
    Coap::addEndpoint(q);
    udp = new QUdpSocket(q);
    QObject::connect(udp, SIGNAL(readyRead()),
                     q,   SLOT(_q_on_udp_ready_read()));

    timerQueue = new TimerQueue(q);
    QObject::connect(timerQueue, SIGNAL(timeout(QByteArray)),
                     q,          SLOT(_q_on_timeout(QByteArray)));

    // register built in content handlers
    Coap::addUnpacker((quint16)CoapMessage::ContentFormat::AppJson,
                      &CoapContentHandlers::unpackJSONContent);
    Coap::addUnpacker((quint16)77, // TODO msgpack content format to config
                      &CoapContentHandlers::unpackMsgPackContent);
}

void CoapEndpointPrivate::loadConfiguration()
{
    QFile file(configurationFilename);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "CoapEndpoint: can't load configuration" << file.errorString();
        qDebug() << "This may be ok, if it is the first time you run your app";
        return;
    }
    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (document.isNull()) {
        qWarning() << "Something wrong with configuration file, check syntax";
        return;
    }
    QVariantMap configuration = document.toVariant().toMap();

    Q_Q(CoapEndpoint);
    interface = QHostAddress(configuration["interface"].toString());
    emit q->interfaceChanged();

    port = configuration["port"].toInt();
    emit q->portChanged();

    if (!bind()) {
        qWarning() << "Can't bind, check configuration";
    }
}

void CoapEndpointPrivate::saveConfiguration()
{
    if (configurationFilename.isEmpty())
        return;

    QFile file(configurationFilename);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "CoapEndpoint: can't save configuration" << file.errorString();
        return;
    }

    QJsonObject map;
    map["interface"] = interface.toString();
    map["port"] = port;
    file.write(QJsonDocument(map).toJson());

    qDebug() << "Config saved";
}

void CoapEndpointPrivate::tx(CoapExchange *fromExchange, CoapMessage &message)
{
    if (message.isRequest())
        txRequest(fromExchange, message);
    else if (message.isResponse())
        txResponse(fromExchange, message);
    else
        txEmpty(fromExchange, message);
}

void CoapEndpointPrivate::txRequest(CoapExchange *fromExchange, CoapMessage &request)
{
    removeExchange(fromExchange); // remove previous data

    if (request.messageId() == 0)
        request.setMessageId((currentMid++) % 65536);

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

    if (request.type() == CoapMessage::Type::Confirmable) {// schedule retransmission
        fromExchange->d_ptr->retransmissionCount = 0;
        timerQueue->addTimer(2000, request.token());
    }

    sendMessage(request);
}

QByteArray CoapEndpointPrivate::generateUniqueToken()
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

void CoapEndpointPrivate::_q_on_timeout(const QByteArray &key)
{
    CoapExchange *exchange = exchangeByToken.value(key, 0);
    if (!exchange)
        return;
    if (++exchange->d_ptr->retransmissionCount == 4) { // give up
        exchange->handleError();
    } else {
        sendMessage(exchange->d_ptr->message);
        timerQueue->addTimer(2000, key);
    }
}

void CoapEndpointPrivate::txResponse(CoapExchange *fromExchange, CoapMessage &response)
{

}

void CoapEndpointPrivate::txEmpty(CoapExchange *fromExchange, CoapMessage &empty)
{
    Q_UNUSED(fromExchange);
    sendMessage(empty);
}

void CoapEndpointPrivate::sendMessage(CoapMessage &message)
{
    QByteArray packed = message.pack();
    qDebug() << "Sending datagram to" << message.address() << message.port();
    udp->writeDatagram(packed, message.address(), message.port());
}

void CoapEndpointPrivate::_q_on_udp_ready_read()
{
    while (udp->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udp->pendingDatagramSize());
        QHostAddress from;
        quint16 fromPort;
        udp->readDatagram(datagram.data(), datagram.size(),
                                &from, &fromPort);
        CoapMessage message;
        message.unpack(datagram);
        message.setAddress(from);
        message.setPort(fromPort);
        qDebug() << "Processing incoming pdu from:" << from.toString() << message;
        if (message.isValid())
            rx(message); // pass to upper level
    }
}

void CoapEndpointPrivate::rx(CoapMessage &message)
{
    if (message.isRequest())
        rxRequest(message);
    else if (message.isResponse())
        rxResponse(message);
    else
        rxEmpty(message);
}

void CoapEndpointPrivate::rxRequest(CoapMessage &request)
{

}

void CoapEndpointPrivate::rxResponse(CoapMessage &response)
{
    if (response.type() == CoapMessage::Type::Reset) {
        qDebug() << "Ignoring response with Type::Reset";
        return;
    }
    CoapExchange *exchange = exchangeByToken.value(response.token(), 0);
    if (exchange) {
        qDebug() << "found exchange" << exchange;
        timerQueue->removeTimer(response.token());
        exchange->handle(response);
    } else {
        qDebug() << "Strange or after observe response received, RST it";
        CoapMessage rst;
        rst.setAddress(response.address());
        rst.setPort(response.port());
        rst.setType(CoapMessage::Type::Reset);
        rst.setToken(response.token());
        rst.setMessageId(response.messageId());
        tx(0, rst);
    }
}

void CoapEndpointPrivate::rxEmpty(CoapMessage &empty)
{

}

void CoapEndpointPrivate::removeExchange(CoapExchange *exchange)
{
    QByteArray token = exchangeByToken.key(exchange);
    if (token.isEmpty())
        return;
    qDebug() << "Removing exchange with token:" << token.toHex();
    exchangeByToken.remove(token);
    timerQueue->removeTimer(token);
}

bool CoapEndpointPrivate::bind()
{
    if (interface.isNull())
        return false;
    if (port == 0) {
        port = 42400;
        while (!udp->bind(interface, port))
            port++;
        return true;
    }
    return udp->bind(interface, port);
}

CoapEndpoint::CoapEndpoint(QObject *parent) :
    QObject(parent), d_ptr(new CoapEndpointPrivate)
{
    Q_D(CoapEndpoint);
    d->q_ptr = this;
    d->setup();
}

CoapEndpoint::CoapEndpoint(CoapEndpointPrivate &dd, QObject *parent) :
    QObject(parent), d_ptr(&dd)
{
    Q_D(CoapEndpoint);
    d->q_ptr = this;
    d->setup();
}

CoapEndpoint::~CoapEndpoint()
{
    if (d_ptr) {
        delete d_ptr;
        d_ptr = 0;
    }
}

bool CoapEndpoint::bind(const QHostAddress &interace, quint16 port)
{
    Q_D(CoapEndpoint);
    setInterface(interace);
    setPort(port);
    return d->bind();
}

bool CoapEndpoint::bind()
{
    Q_D(CoapEndpoint);
    return d->bind();
}

void CoapEndpoint::attachConfiguration(const QString &filename)
{
    Q_D(CoapEndpoint);
    d->configurationFilename = filename;
    d->loadConfiguration();
    d->configurationSaveTimer.setSingleShot(true);
    connect(&d->configurationSaveTimer, &QTimer::timeout, [d](){d->saveConfiguration();});
}

QHostAddress CoapEndpoint::interface() const
{
    Q_D(const CoapEndpoint);
    return d->interface;
}

void CoapEndpoint::setInterface(const QHostAddress &interface)
{
    Q_D(CoapEndpoint);
    d->interface = interface;
    emit interfaceChanged();
    d->configurationSaveTimer.start(100);
}

QString CoapEndpoint::interfaceString() const
{
    Q_D(const CoapEndpoint);
    return d->interface.toString();
}

void CoapEndpoint::setInterfaceString(const QString &interfaceString)
{
    Q_D(CoapEndpoint);
    d->interface = QHostAddress(interfaceString);
    emit interfaceChanged();
}

quint16 CoapEndpoint::port() const
{
    Q_D(const CoapEndpoint);
    return d->port;
}

void CoapEndpoint::setPort(quint16 port)
{
    Q_D(CoapEndpoint);
    d->port = port;
    emit portChanged();
    d->configurationSaveTimer.start(100);
}

#include "moc_coapendpoint.cpp"
