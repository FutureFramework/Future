#include "coap.hpp"
#include "contenthandlers.h"
#include "message.hpp"

#include <QList>
#include <QDebug>

struct CoapPrivate {
    CoapPrivate() {}
    ~CoapPrivate() {}
    QList<iotlib::coap::Stack *> stacks;
    QHash<quint16, payload_unpacker_f> unpackers;
};

Q_GLOBAL_STATIC(CoapPrivate, coap_private)

Coap::Coap()
{ }

iotlib::coap::Stack *Coap::defaultStack()
{
    CoapPrivate *d = coap_private;
    if (!d || d->stacks.isEmpty()) {
        qWarning() << "Coap::defaultEndpoint(): no default endpoint";
        return 0;
    }
    return d->stacks.at(0);
}

void Coap::addStack(iotlib::coap::Stack *stack)
{
    CoapPrivate *d = coap_private;
    if (!d)
        return;
    d->stacks.append(stack);
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
