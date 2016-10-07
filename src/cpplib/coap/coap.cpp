#include "coap.hpp"
#include "contenthandlers.h"
#include "message.hpp"

#include <QList>
#include <QDebug>

struct CoapPrivate {
    CoapPrivate() {}
    ~CoapPrivate() {}
    QList<CoapEndpoint *> endpoints;
    QHash<quint16, payload_unpacker_f> unpackers;
};

Q_GLOBAL_STATIC(CoapPrivate, coap_private)

Coap::Coap()
{ }

CoapEndpoint *Coap::defaultEndpoint()
{
    CoapPrivate *d = coap_private;
    if (!d || d->endpoints.isEmpty()) {
        qWarning() << "Coap::defaultEndpoint(): no default endpoint";
        return 0;
    }
    return d->endpoints.at(0);
}

void Coap::addEndpoint(CoapEndpoint *endpoint)
{
    CoapPrivate *d = coap_private;
    if (!d)
        return;
    d->endpoints.append(endpoint);
}

void Coap::removeEndpoint(CoapEndpoint *endpoint)
{
    CoapPrivate *d = coap_private;
    if (!d)
        return;
    d->endpoints.removeOne(endpoint);
}

QList<CoapEndpoint *> Coap::endpoints()
{
    CoapPrivate *d = coap_private;
    if (!d)
        return QList<CoapEndpoint *>();
    return d->endpoints;
}

void Coap::addUnpacker(quint16 contentFormat, payload_unpacker_f unpacker)
{
    CoapPrivate *d = coap_private;
    if (!d)
        return;
    d->unpackers.insert(contentFormat, unpacker);
}

payload_unpacker_f Coap::unpacker(quint16 contentFormat)
{
    CoapPrivate *d = coap_private;
    if (!d)
        return 0;
    return d->unpackers.value(contentFormat, 0);
}
