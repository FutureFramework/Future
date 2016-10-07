#ifndef UDPENDPOINT_H
#define UDPENDPOINT_H

#include "endpointbase.hpp"
#include "../settings.h"

#include <QObject>

class QUdpSocket;

namespace iotlib {
namespace coap {

class UdpEndpoint : public EndpointBase
{
    Q_OBJECT
public:
    UdpEndpoint(Settings *settings, QObject *parent = 0);

public slots:
     void send(const Message &coapMessage);

private slots:
    void onSettingsChanged();
    void onReadyRead();

private:
    Settings *m_settings;
    QUdpSocket *m_socket;
};

} // coap
} // iotlib

#endif // UDPENDPOINT_H
