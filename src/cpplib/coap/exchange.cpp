#include "exchange.hpp"
#include "exchange_p.hpp"
#include "stack_p.hpp"
#include "coap.hpp"

#include <QJSValue>

iotlib::coap::ExchangePrivate::ExchangePrivate() :
    status(iotlib::coap::Exchange::Initial),
    sendAfterLookup(false),
    deleteAfterComplete(false),
    observe(false)
{
}

iotlib::coap::ExchangePrivate::~ExchangePrivate()
{

}

void iotlib::coap::ExchangePrivate::setStatus(iotlib::coap::Exchange::Status status)
{
    Q_Q(iotlib::coap::Exchange);
    this->status = status;
    emit q->statusChanged();
}

void iotlib::coap::ExchangePrivate::_q_looked_up(const QHostInfo &info)
{
    qDebug() << "lookup complete" << info.addresses();
    Q_Q(iotlib::coap::Exchange);
    if (info.error() == QHostInfo::NoError) {
        Address address = message.address();
        address.setHostAddress(info.addresses()[0]);
        message.setAddress(address);
        if (sendAfterLookup) {
            setStatus(iotlib::coap::Exchange::InProgress);
            q->send(message);
        } else {
            setStatus(iotlib::coap::Exchange::Ready);
        }
    } else {
        setStatus(iotlib::coap::Exchange::LookupFailed);
    }
}

bool iotlib::coap::ExchangePrivate::isReady()
{
    if (status == iotlib::coap::Exchange::InProgress) {
        return false;
    } else {
        return true;
    }
}

iotlib::coap::Exchange::Exchange(QObject *parent) :
    QObject(parent), d_ptr(new iotlib::coap::ExchangePrivate)
{
    Q_D(iotlib::coap::Exchange);
    d->q_ptr = this;
    d->stack = Coap::defaultStack();
    if (!parent)
        setParent(d->stack);
}

iotlib::coap::Exchange::Exchange(iotlib::coap::ExchangePrivate &dd, QObject *parent) :
    QObject(parent), d_ptr(&dd)
{
    Q_D(iotlib::coap::Exchange);
    d->q_ptr = this;
    d->stack = Coap::defaultStack();
    if (!parent)
        setParent(d->stack);
}

iotlib::coap::Exchange::~Exchange()
{
    if (d_ptr->status == Lookup ||
            d_ptr->status == InProgress)
        qWarning() << "iotlib::coap::Exchange to" << urlString() << "is destroyed in" << d_ptr->status << "state";
    if (d_ptr)
        delete d_ptr;
    //Q_D(iotlib::coap::Exchange);
//    d->endpoint->d_ptr->remove_iotlib::coap::Exchange(this);
}

void iotlib::coap::Exchange::setUrl(const QUrl &url)
{
    Q_D(iotlib::coap::Exchange);
    if (status() == InProgress) {
        qWarning() << "iotlib::coap::Exchange is InProgress, can't change uri";
        return;
    }
    if (d->message.address().hostAddress().isNull()) {
        QHostAddress hostAddress = QHostAddress(url.host());
        if (hostAddress.isNull()) {
            d->setStatus(Lookup);
            QHostInfo::lookupHost(url.host(), this, SLOT(_q_looked_up(QHostInfo)));
        } else {
            d->message.setAddress(Address(hostAddress, 0));
        }
    }
    d->message.setUrl(url);
    Address address = d->message.address();
    address.setPort(url.port(5683));
    d->message.setAddress(address);
    d->url = url;
    emit urlChanged();
}

QUrl iotlib::coap::Exchange::url() const
{
    Q_D(const iotlib::coap::Exchange);
    return d->url;
}

void iotlib::coap::Exchange::setUrlString(const QString &urlString)
{
    setUrl(QUrl(urlString));
}

QString iotlib::coap::Exchange::urlString() const
{
    Q_D(const iotlib::coap::Exchange);
    return d->url.toString();
}

iotlib::coap::Exchange::Status iotlib::coap::Exchange::status() const
{
    Q_D(const iotlib::coap::Exchange);
    return d->status;
}

void iotlib::coap::Exchange::get()
{
    Q_D(iotlib::coap::Exchange);
    if (!d->isReady()) {
        qWarning() << "iotlib::coap::Exchange::get() failed, iotlib::coap::Exchange is InProgress state";
        return;
    }

    d->message.setCode(Message::Code::Get);
    d->message.setType(Message::Type::Confirmable);
    if (status() == Lookup) {
        d->sendAfterLookup = true;
    } else {
        d->setStatus(InProgress);
        send(d->message);
    }
}

void iotlib::coap::Exchange::observe()
{
    Q_D(iotlib::coap::Exchange);
    if (!d->isReady())
        return;
    d->setStatus(InProgress);
    d->observe = true;

    Message get;
    get.setCode(Message::Code::Get);
    get.setType(Message::Type::Confirmable);
    get.addOption(Message::OptionType::Observe);
    get.setUrl(d->url);
    send(get);
}

void iotlib::coap::Exchange::cancel()
{
    Q_D(iotlib::coap::Exchange);
    d->stack->d_ptr->removeExchange(this);
    d->setStatus(Ready);
}

QByteArray iotlib::coap::Exchange::contentRaw() const
{
    Q_D(const iotlib::coap::Exchange);
    return d->message.content();
}

QVariant iotlib::coap::Exchange::content() const
{
    Q_D(const iotlib::coap::Exchange);
    payload_unpacker_f unpacker = Coap::unpacker((quint16)d->message.contentFormat());
    if (!unpacker) {
        qWarning() << "iotlib::coap::Exchange::payload(): no unpacker for content format" << d->message.contentFormat();
        return QVariant();
    }
    return unpacker(d->message.content());
}

void iotlib::coap::Exchange::deleteAfterComplete()
{
    Q_D(iotlib::coap::Exchange);
    d->deleteAfterComplete = true;
}

void iotlib::coap::Exchange::handle(Message &message)
{
    Q_D(iotlib::coap::Exchange);
    d->message = message;
    if (message.code() == Message::Code::Content) {
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

void iotlib::coap::Exchange::handleError()
{
    Q_D(iotlib::coap::Exchange);
    emit timeout();
    d->setStatus(TimedOut);

    if (d->deleteAfterComplete)
        deleteLater();
}

void iotlib::coap::Exchange::send(Message &message)
{
    Q_D(iotlib::coap::Exchange);
    if (!d->stack) {
        qWarning() << "Can't send a message without CoapEndpoint, create it first";
        return;
    }
    d->stack->d_ptr->tx(this, message);
}

#include "moc_exchange.cpp"
