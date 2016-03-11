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

    QString host;
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
        d->host = match.captured(2);
        d->path = match.captured(3);
    }
}

CoapUri::CoapUri(const CoapUri &other) :
    d(other.d)
{
}

CoapUri &CoapUri::operator =(const CoapUri &other)
{
    d = other.d;
    return *this;
}

CoapUri::~CoapUri()
{
}

void CoapUri::setHost(const QString &host)
{
    d->host = host;
}

QString CoapUri::host() const
{
    return d->host;
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

