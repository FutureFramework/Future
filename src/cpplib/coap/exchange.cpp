#include "exchange.hpp"
#include "exchange_p.hpp"
#include "stack_p.hpp"
#include "coap.hpp"

#include <QJSValue>

CoapExchangePrivate::CoapExchangePrivate() :
    status(CoapExchange::Initial),
    sendAfterLookup(false),
    deleteAfterComplete(false),
    observe(false)
{
}

CoapExchangePrivate::~CoapExchangePrivate()
{

}

void CoapExchangePrivate::setStatus(CoapExchange::Status status)
{
    Q_Q(CoapExchange);
    this->status = status;
    emit q->statusChanged();
}

void CoapExchangePrivate::_q_looked_up(const QHostInfo &info)
{
    qDebug() << "lookup complete" << info.addresses();
    Q_Q(CoapExchange);
    if (info.error() == QHostInfo::NoError) {
        QHostAddress address = info.addresses()[0];
        message.setAddress(address);
        if (sendAfterLookup) {
            setStatus(CoapExchange::InProgress);
            q->send(message);
        } else {
            setStatus(CoapExchange::Ready);
        }
    } else {
        setStatus(CoapExchange::LookupFailed);
    }
}

bool CoapExchangePrivate::isReady()
{
    if (status == CoapExchange::InProgress) {
        return false;
    } else {
        return true;
    }
}

CoapExchange::CoapExchange(QObject *parent) :
    QObject(parent), d_ptr(new CoapExchangePrivate)
{
    Q_D(CoapExchange);
    d->q_ptr = this;
    d->endpoint = Coap::defaultEndpoint();
    if (!parent)
        setParent(d->endpoint);
}

CoapExchange::CoapExchange(CoapExchangePrivate &dd, QObject *parent) :
    QObject(parent), d_ptr(&dd)
{
    Q_D(CoapExchange);
    d->q_ptr = this;
    d->endpoint = Coap::defaultEndpoint();
    if (!parent)
        setParent(d->endpoint);
}

CoapExchange::~CoapExchange()
{
    if (d_ptr->status == Lookup ||
            d_ptr->status == InProgress)
        qWarning() << "Exchange to" << urlString() << "is destroyed in" << d_ptr->status << "state";
    if (d_ptr)
        delete d_ptr;
    //Q_D(CoapExchange);
//    d->endpoint->d_ptr->remove_exchange(this);
}

void CoapExchange::setUrl(const QUrl &url)
{
    Q_D(CoapExchange);
    if (status() == InProgress) {
        qWarning() << "Exchange is InProgress, can't change uri";
        return;
    }
    if (d->message.address().isNull()) {
        QHostAddress hostAddress = QHostAddress(url.host());
        if (hostAddress.isNull()) {
            d->setStatus(Lookup);
            QHostInfo::lookupHost(url.host(), this, SLOT(_q_looked_up(QHostInfo)));
        } else {
            d->message.setAddress(hostAddress);
        }
    }
    d->message.setUrl(url);
    d->message.setPort(url.port(5683));
    d->url = url;
    emit urlChanged();
}

QUrl CoapExchange::url() const
{
    Q_D(const CoapExchange);
    return d->url;
}

void CoapExchange::setUrlString(const QString &urlString)
{
    setUrl(QUrl(urlString));
}

QString CoapExchange::urlString() const
{
    Q_D(const CoapExchange);
    return d->url.toString();
}

CoapExchange::Status CoapExchange::status() const
{
    Q_D(const CoapExchange);
    return d->status;
}

void CoapExchange::get()
{
    Q_D(CoapExchange);
    if (!d->isReady()) {
        qWarning() << "CoapExchange::get() failed, exchange is InProgress state";
        return;
    }

    d->message.setCode(CoapMessage::Code::Get);
    d->message.setType(CoapMessage::Type::Confirmable);
    if (status() == Lookup) {
        d->sendAfterLookup = true;
    } else {
        d->setStatus(InProgress);
        send(d->message);
    }
}

void CoapExchange::observe()
{
    Q_D(CoapExchange);
    if (!d->isReady())
        return;
    d->setStatus(InProgress);
    d->observe = true;

    CoapMessage get;
    get.setCode(CoapMessage::Code::Get);
    get.setType(CoapMessage::Type::Confirmable);
    get.addOption(CoapMessage::OptionType::Observe);
    get.setUrl(d->url);
    send(get);
}

void CoapExchange::cancel()
{
    Q_D(CoapExchange);
    d->endpoint->d_ptr->removeExchange(this);
    d->setStatus(Ready);
}

QByteArray CoapExchange::contentRaw() const
{
    Q_D(const CoapExchange);
    return d->message.content();
}

QVariant CoapExchange::content() const
{
    Q_D(const CoapExchange);
    payload_unpacker_f unpacker = Coap::unpacker((quint16)d->message.contentFormat());
    if (!unpacker) {
        qWarning() << "CoapExchange::payload(): no unpacker for content format" << d->message.contentFormat();
        return QVariant();
    }
    return unpacker(d->message.content());
}

void CoapExchange::deleteAfterComplete()
{
    Q_D(CoapExchange);
    d->deleteAfterComplete = true;
}

void CoapExchange::handle(CoapMessage &message)
{
    Q_D(CoapExchange);
    d->message = message;
    if (message.code() == CoapMessage::Code::Content) {
        //d->lambdaCompleted();
        if (!d->observe) {
            emit completed();
            d->setStatus(Completed);
        }
    } else {
        if (!d->observe) {
            emit completed();
            d->setStatus(Completed);
        }
    }

    if (d->status == Completed && d->deleteAfterComplete)
        deleteLater();
}

void CoapExchange::handleError()
{
    Q_D(CoapExchange);
    emit timeout();
    d->setStatus(TimedOut);

    if (d->deleteAfterComplete)
        deleteLater();
}

void CoapExchange::send(CoapMessage &message)
{
    Q_D(CoapExchange);
    if (!d->endpoint) {
        qWarning() << "Can't send a message without CoapEndpoint, create it first";
        return;
    }
    d->endpoint->d_ptr->tx(this, message);
}

#include "moc_coapexchange.cpp"
