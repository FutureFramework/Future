#ifndef COAP_EXCHANGE_H
#define COAP_EXCHANGE_H

#include "iotlib_global.h"
#include "message.hpp"

namespace iotlib {
namespace coap {

class Stack;
class StackPrivate;
class ExchangePrivate;

/**
 * @brief The Exchange class represents a logical conversation between CoAP client and server.
 * Logical means across different message id's and even tokens for block transfer.
 */
class IOTLIB_SHARED_EXPORT Exchange : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString uri READ urlString WRITE setUrlString NOTIFY urlChanged)
public:
    /**
     * @brief Exchange through default endpoint
     */
    Exchange(QObject *parent = 0);
    virtual ~Exchange();
    /**
     * @brief setUri setts uri of a resource(-s) we are going to talk to.
     * @param uri contains host address, port, path and other data
     * @see CoapUri
     */
    void setUrl(const QUrl &url);
    /**
     * @brief uri returns uri of a resource we are talking with.
     * @return
     */
    QUrl url() const;
    void setUrlString(const QString &urlString);
    QString urlString() const;

    /**
     * @brief The Status enum
     */
    enum Status {
        Initial,     ///< After creation
        Lookup,      ///< Performing DNS lookup (only for host names)
        Ready,       ///< Ready for making request or observe
        InProgress,  ///< Performing request, observing
        Completed,   ///< Answer received, or abort() called
        TimedOut,    ///< Host not answered even after retransmissions
        LookupFailed ///< DNS lookup failed
    };
    Q_ENUM(Status)
    /**
     * @brief status
     * @return status of this exchange
     */
    Status status() const;

    Q_INVOKABLE void get();
    Q_INVOKABLE void observe();
    Q_INVOKABLE void cancel();

    QByteArray contentRaw() const;
    QVariant content() const;

    /**
     * @brief deleteAfterComplete controls lifetime of Exchange object.
     * Call this function to automatically remove exchange after answer war received or timeout occured.
     * Default value is false
     */
    void deleteAfterComplete();

signals:
    void statusChanged();
    void urlChanged();
    void completed();
    void timeout();
protected:
    /**
     * @brief handle is called when PDU, associated with this exchange arrives
     * @param pdu
     */
    virtual void handle(Message &message);
    virtual void handleError();
    /**
     * @brief send sends pdu to remote server or client
     * @param pdu
     */
    void send(Message &message);

    Exchange(ExchangePrivate &dd, QObject *parent = 0);
    ExchangePrivate *d_ptr;
private:
    Q_DECLARE_PRIVATE(Exchange)
    Q_PRIVATE_SLOT(d_func(), void _q_looked_up(const QHostInfo &))
    friend class Stack;
    friend class StackPrivate;
};

} // coap
} // iotlib

#endif // COAP_EXCHANGE_H
