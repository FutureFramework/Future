#ifndef COAPCONTENTHANDLERS_H
#define COAPCONTENTHANDLERS_H

#include <QVariant>

class CoapContentHandlers
{
public:
    static QVariant unpackJSONContent(const QByteArray &data);
    static QVariant unpackMsgPackContent(const QByteArray &data);

};

#endif // COAPCONTENTHANDLERS_H
