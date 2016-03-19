#ifndef COAPURI_H
#define COAPURI_H

#include "../iotlib_global.h"

#include <QHostAddress>
#include <QSharedDataPointer>

class CoapUriPrivate;
class IOTLIB_SHARED_EXPORT CoapUri
{
    Q_GADGET
    Q_PROPERTY(quint16 port READ port WRITE setPort)
    Q_PROPERTY(QString path READ path WRITE setPath)

public:
    /**
     * @brief CoapUri construct an invalid uri
     */
    CoapUri();
    CoapUri(const QString &fromString);
    CoapUri(const CoapUri &other);
    CoapUri &operator =(const CoapUri &other);
    ~CoapUri();

    void setHostName(const QString &hostName); // ip or name
    QString hostName() const; // host name even if it is resolved already
    void setHost(const QHostAddress &address); // only ip
    QHostAddress host() const;
    void setPort(quint16 port);
    quint16 port() const;
    void setPath(const QString &path);
    QString path() const;
    bool isSecure() const; // true if coaps or coaps+lwm2m
    bool isResolved() const; // ifip->true, ifname in setUri->startLookup->uri.setHost(QHostAddress);

private:
    QSharedDataPointer<CoapUriPrivate> d;
};

Q_DECLARE_METATYPE(CoapUri)

#endif // COAPURI_H
