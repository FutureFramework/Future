#ifndef COAPENDPOINT_P_H
#define COAPENDPOINT_P_H

#include "stack.hpp"

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

namespace iotlib {
namespace coap {

class MidAddressPortKey
{
public:
    MidAddressPortKey(quint32 messageId,
                      const QHostAddress &address = QHostAddress(),
                      quint16 port = 0) :
        m_messageId(messageId), m_address(address), m_port(port)
    { }


private:
    quint32 m_messageId;
    QHostAddress m_address;
    quint16 m_port;

    friend bool operator==(const MidAddressPortKey &m1, const MidAddressPortKey &m2);
    friend inline uint qHash(const MidAddressPortKey &key, uint seed);
};

inline bool operator==(const MidAddressPortKey &m1, const MidAddressPortKey &m2)
{
    return (m1.m_messageId == m2.m_messageId) &&
           (m1.m_address   == m2.m_address) &&
           (m1.m_port      == m2.m_port);
}

inline uint qHash(const MidAddressPortKey &key, uint seed)
{
    return qHash(key.m_address, seed) ^ key.m_messageId ^ key.m_port;
}

class TimerQueue;
class Exchange;
class StackPrivate
{
    Q_DECLARE_PUBLIC(Stack)
public:
    StackPrivate();
    virtual ~StackPrivate();

    void setup();

    /**
     * @brief tx towards network, pass to lower layers
     * @param exchange exchange that message belongs to
     * @param message pdu + address
     */
    void tx(Exchange *fromExchange, Message &message);
    void txRequest(Exchange *fromExchange, Message &request);
    void txResponse(Exchange *fromExchange, Message &response);
    void txEmpty(Exchange *fromExchange, Message &empty);
    /**
     * @brief rx from network, pass to upper layers
     * @param exchange
     * @param message
     */
    void rx(Message &message);
    void rxRequest(Message &request);
    void rxResponse(Message &response);
    void rxEmpty(Message &empty);
    Stack *q_ptr;

    void removeExchange(Exchange *exchange);

    // Network
    void _q_on_message_received(Message &message);
    void sendMessage(Message &message);

    // Classification
    QByteArray generateUniqueToken();
    quint16 currentMid;
    QHash<MidAddressPortKey, Exchange *> exchangeByMid;
    QHash<QByteArray, Exchange *> exchangeByToken;

    // Reliability
    TimerQueue *timerQueue;
    void _q_on_timeout(const QByteArray &key);
};

} // coap
} // iotlib

#endif // COAPENDPOINT_P_H
