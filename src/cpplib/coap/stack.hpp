#ifndef COAP_STACK_H
#define COAP_STACK_H

#include "../iotlib_global.h"
#include "message.hpp"

#include <QObject>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QUrl>


namespace iotlib {
namespace coap {

class CoapExchange;
class StackPrivate;
/** @file */
/**
 * @brief The iotlib::coap::Stack class
 * Handles all the CoAP communications
 */
class IOTLIB_SHARED_EXPORT Stack : public QObject
{
    Q_OBJECT
public:


    /**
     * @brief iotlib::coap::Stack Initialize endpoint
     * @param endpointType Client or ClientServer @see Type
     * @param endpointName Name of this endpoint, useful when there is more that one endpoint
     * @param parent
     * Note that endpointName is used only to find this endpoint later by calling Coap::endpoint("name")
     * iotlib::coap::StackInfo::name() is in contrast used in communications with other CoAP nodes
     */
    Stack(QObject *parent = 0);

    /**
     * @brief ~iotlib::coap::Stack Destroy everything and close sockets
     * If requestCertificate() was previously called, then before destroying multicast message is sended out,
     * to group provision, indicating that endpoint is shutting down
     */
    virtual ~Stack();

    /**
     * @brief bind Bind on specific interface and port
     * @param address QHostAddress::LocalHost for example
     * @param port
     * @return true on success
     * If port is 0 then 5683 will be used in a case of ClientServer mode and some free port in a case of Client mode.
     * Note that multicast interface is configured using @see bindMulticast method
     */


    /**
     * @brief bindMulticast Bind for multicast exchanges
     * @param groupAddress
     * @param iface
     * @return true on success
     * Note that enabling multicast without any protection is not recommended by RFC7252 11.3
     */


    /**
     * @brief setEndpointInfo Provide an info about this endpoint
     * @param endpointInfo Class that contains endpoint name, endpoint groups, etc
     */
//    void setEndpointInfo(const iotlib::coap::StackInfo &endpointInfo);

    /**
     * @brief setTransmissionParameters Configure transmissions timeouts, speed, etc
     * @param parameters See CoAP section 4.8
     */
//    void setExchangeParameters(const CoapExchangeParameters &parameters);

    /**
     * @brief setCertificate Set certificate for encrypting all the traffic
     * @param certificateFile PKCS file containing root ca certificate, endpoint certificate and key
     */


    /**
     * @brief startProvision initiates a provision process.
     * If certificate is already exist it will be loaded through setCertificate(),
     * otherwise multicast request to group "provision" will be maded,
     * then administrator will decide issue the certificate to specific endpoint or not.
     * Additionally it creates several resources, that used in provision.
     * Loads exchange parameters from exchangeparameters.json and changes them through
     * resource /_provision/exchange-parameters.
     * /_provision/certificate is used for certificate reception and further updates.
     *
     * Note that bindMulticast() or routeRequestsToProxy() must be called for this method to work,
     * and that provision server must be running somewhere in your network
     * @todo Provision server
     * @todo certificate is stored in unencrypted way, and may be stolen, need a solution here.
     */


    /**
     * @brief routeRequestsToProxy Perform all coap:// and coaps:// exchanges through proxy
     * @param proxyAddress
     * This is very useful on mobile platforms, when device may be used
     * in private network (where all the other CoAP nodes are) or from outside,
     * in this case all exchanges will be transparently routed to proxy
     * Note that proxy must be configured properly for this method to function
     * @todo Such a proxy
     */

protected:
    StackPrivate * d_ptr;
    Stack(StackPrivate &dd, QObject *parent);
private:
    Q_DECLARE_PRIVATE(iotlib::coap::Stack)
    Q_PRIVATE_SLOT(d_func(), void _q_on_message_received(Message &message))
    Q_PRIVATE_SLOT(d_func(), void _q_on_timeout(const QByteArray &))
    friend class Exchange;
    friend class ExchangePrivate;
};

} // coap
} // iotlib

#endif // COAP_STACK_H
