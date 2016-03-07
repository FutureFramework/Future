#include "coap.hpp"

#include <QList>
#include <QDebug>

struct CoapPrivate {
    CoapPrivate() {}
    ~CoapPrivate() {}
    QList<CoapEndpoint *> endpoints;
};

Q_GLOBAL_STATIC(CoapPrivate, coap_private)

Coap::Coap()
{
}

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
