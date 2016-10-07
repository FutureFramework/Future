#include "udpendpoint.h"

#include <QUdpSocket>

iotlib::coap::UdpEndpoint::UdpEndpoint(iotlib::Settings *settings, QObject *parent) :
    iotlib::coap::EndpointBase(parent), m_settings(settings)
{
    m_socket = new QUdpSocket(this);
    connect(m_socket, &QUdpSocket::readyRead,
            this,     &UdpEndpoint::onReadyRead);

    connect(settings, &Settings::settingsChanged,
            this,     &UdpEndpoint::onSettingsChanged);
}

void iotlib::coap::UdpEndpoint::send(const iotlib::coap::Message &coapMessage)
{
    QByteArray packed = coapMessage.pack();
    qDebug() << "Sending datagram to" << coapMessage.address().hostAddress()
             << coapMessage.address().port();
    m_socket->writeDatagram(packed,
                            coapMessage.address().hostAddress(),
                            coapMessage.address().port());
}

void iotlib::coap::UdpEndpoint::onSettingsChanged()
{
    bool bind = m_settings->get("bind").toBool();
    if (!bind) {
        m_socket->abort();
        return;
    }

    QHostAddress interface(m_settings->get("interface").toString());
    quint16 port = static_cast<quint16>(m_settings->get("port").toUInt());

    if (m_socket->localAddress() != interface || m_socket->localPort() != port) {
        if (port == 0) {
            port = 42400;
            while (!m_socket->bind(interface, port))
                port++;
            return;
        }
        if (!m_socket->bind(interface, port)) {
            qWarning() << "Bind failed:" << m_socket->errorString();
        }
    }
}

void iotlib::coap::UdpEndpoint::onReadyRead()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(m_socket->pendingDatagramSize()));
        QHostAddress from;
        quint16 fromPort;
        m_socket->readDatagram(datagram.data(), datagram.size(),
                                &from, &fromPort);
        Message message;
        message.unpack(datagram);
        message.setAddress(Address(from, fromPort));
        qDebug() << "Processing incoming pdu from:" << from.toString() << message;
        if (message.isValid())
            emit received(message);
    }
}
