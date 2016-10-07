#include "contenthandlers.h"
#include <QJsonDocument>
#include "qmsgpack/msgpack.h"

QVariant CoapContentHandlers::unpackJSONContent(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    if (document.isNull())
        return QVariant();
    return document.toVariant();
}

QVariant CoapContentHandlers::unpackMsgPackContent(const QByteArray &data)
{
    return MsgPack::unpack(data);
}
