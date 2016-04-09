#ifndef COAP_H
#define COAP_H

#include "../iotlib_global.h"

#include <QVariant>
#include <QList>

typedef QVariant (*payload_unpacker_f)(const QByteArray &packed);
typedef QByteArray (*payload_packer_f)(const QVariant &data);

class CoapEndpoint;
class IOTLIB_SHARED_EXPORT Coap
{
public:
    static CoapEndpoint *defaultEndpoint();
    static void addEndpoint(CoapEndpoint *endpoint);
    static void removeEndpoint(CoapEndpoint *endpoint);
    static QList<CoapEndpoint *> endpoints();

    static void addUnpacker(quint16 contentFormat, payload_unpacker_f unpacker);
    static payload_unpacker_f unpacker(quint16 contentFormat);
private:
    Coap();
};

#endif // COAP_H
