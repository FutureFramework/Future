#include "coapuri.hpp"

#include <QRegularExpression>

class CoapUriPrivate : public QSharedData
{
public:
    CoapUriPrivate()
    {
        port = 5683;
    }

    CoapUriPrivate(const CoapUriPrivate &other) :
        QSharedData(other)
    {
    }

    ~CoapUriPrivate()
    {
    }

    QHostAddress hostAddress;
    QString hostName;
    quint16 port;
    QString path;
};

CoapUri::CoapUri() :
    d(new CoapUriPrivate)
{  
}

CoapUri::CoapUri(const QString &fromString) :
    d(new CoapUriPrivate)
{
    QRegularExpression re(R"(^(coaps?:\/\/)?([^\/\s]+)(\/.*)?$)");
    QRegularExpressionMatch match = re.match(fromString);
    if (match.hasMatch()) {
        d->hostName = match.captured(2);
        d->hostAddress = QHostAddress(d->hostName);
        d->path = match.captured(3);
    }
}

CoapUri::CoapUri(const CoapUri &other) :
    d(other.d)
{ }

CoapUri &CoapUri::operator =(const CoapUri &other)
{
    d = other.d;
    return *this;
}

CoapUri::~CoapUri()
{ }

void CoapUri::setHostName(const QString &hostName)
{
    d->hostName = hostName;
    d->hostAddress = QHostAddress(hostName);
}

QString CoapUri::hostName() const
{
    return d->hostName;
}

void CoapUri::setHost(const QHostAddress &host)
{
    d->hostAddress = host;
}

QHostAddress CoapUri::host() const
{
    return d->hostAddress;
}

void CoapUri::setPort(quint16 port)
{
    d->port = port;
}

quint16 CoapUri::port() const
{
    return d->port;
}

void CoapUri::setPath(const QString &path)
{
    d->path = path;
}

QString CoapUri::path() const
{
    return d->path;
}

