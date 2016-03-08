#ifndef COAP_H
#define COAP_H

#include "../iotlib_global.h"

#include <QList>

class CoapEndpoint;
class IOTLIB_SHARED_EXPORT Coap
{
public:
    static CoapEndpoint *defaultEndpoint();
    static void addEndpoint(CoapEndpoint *endpoint);
    static void removeEndpoint(CoapEndpoint *endpoint);
    static QList<CoapEndpoint *> endpoints();
private:
    Coap();
};

#endif // COAP_H
