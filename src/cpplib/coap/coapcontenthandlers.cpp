#include "coapcontenthandlers.h"
#include <QJsonDocument>

QVariant CoapContentHandlers::unpackJSONContent(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    if (document.isNull())
        return QVariant();
    return document.toVariant();
}
