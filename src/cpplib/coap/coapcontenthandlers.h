#ifndef COAPCONTENTHANDLERS_H
#define COAPCONTENTHANDLERS_H

#include <QVariant>

class CoapContentHandlers
{
public:

    static QVariant unpackJSONContent(const QByteArray &data);

};

#endif // COAPCONTENTHANDLERS_H
