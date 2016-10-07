#ifndef COAP_MESSAGE_H
#define COAP_MESSAGE_H

#include "../iotlib_global.h"
#include "coap.hpp"

#include <QObject>
#include <QByteArray>
#include <QSharedDataPointer>
#include <QHostAddress>
#include <QUrl>

namespace iotlib {
namespace coap {

class MessagePrivate;
class Option;
class Address;
class IOTLIB_SHARED_EXPORT Message
{
    Q_GADGET
public:
    Message();
    Message(const QByteArray &array);
    Message(const Message &other);
    Message &operator=(const Message &other);
    ~Message();

    enum class Type : quint8 {
    Confirmable     = 0x00,
    NonConfirmable  = 0x01,
    Acknowledgement = 0x02,
    Reset           = 0x03
    };
    Q_ENUM(Type)

    enum class Code : quint8 {
    Empty                    = 0x00,
    Get                      = 0x01,
    Post                     = 0x02,
    Put                      = 0x03,
    Delete                   = 0x04,
    Created                  = 0x41,
    Deleted                  = 0x42,
    Valid                    = 0x43,
    Changed                  = 0x44,
    Content                  = 0x45,
    BadRequest               = 0x80,
    Unauthorized             = 0x81,
    BadOption                = 0x82,
    Forbidden                = 0x83,
    NotFound                 = 0x84,
    MethodNotAllowed         = 0x85,
    NotAcceptable            = 0x86,
    PreconditionFailed       = 0x8c,
    RequestEntityTooLarge    = 0x8d,
    UnsupportedContentFormat = 0x8f,
    InternalServerError      = 0xa0,
    NotImplemented           = 0xa1,
    BadGateway               = 0xa2,
    ServiceUnavailabl        = 0xa3,
    GatewayTimeout           = 0xa4,
    ProxyingNotSupported     = 0xa5,
    UndefinedCode            = 0xff
    };
    Q_ENUM(Code)

    enum class OptionType : quint16 {
    IfMatch       = 1,
    UriHost       = 3,
    Etag          = 4,
    IfNoneMatch   = 5,
    Observe       = 6,
    UriPort       = 7,
    LocationPath  = 8,
    UriPath       = 11,
    ContentFormat = 12,
    MaxAge        = 14,
    UriQuery      = 15,
    Accept        = 17,
    LocationQuery = 20,
    Block2        = 23,
    Block1        = 27,
    Size2         = 28,
    ProxyUri      = 35,
    ProxyScheme   = 39,
    Size1         = 60
    };
    Q_ENUM(OptionType)

    enum class ContentFormat : quint16 {
    TextPlain = 0,
    AppLink   = 40,
    AppXml    = 41,
    AppOctet  = 42,
    AppExi    = 47,
    AppJson   = 50
    };
    Q_ENUM(ContentFormat)

    void setVersion(quint8 version);
    quint8 version() const;

    void setType(Type type);
    Type type() const;

    void setCode(Code code);
    Code code() const;
    bool isEmpty() const;
    bool isRequest() const;
    bool isResponse() const;

    void setToken(const QByteArray &token);
    void setToken(const char *token, quint8 length);
    QByteArray token() const;

    void setMessageId(quint16 id);
    quint16 messageId() const;

    void addOption(OptionType optionType, const QByteArray &data = QByteArray());
    QList<Option> options() const;
    int optionsCount() const;
    Option option(int idx) const;

    void setUrl(const QUrl &url);
    QUrl url() const;

    void setContentFormat(ContentFormat format);
    ContentFormat contentFormat() const;

    void setContent(const QByteArray &content);
    QByteArray content() const;

    QByteArray pack() const;
    void unpack(const QByteArray &packed);

    Address address() const;
    void setAddress(const Address &address);



    QString errorString() const;
    bool isValid() const;
    bool isNull() const;


private:
    QSharedDataPointer<MessagePrivate> d;
};

class Address
{
public:
    Address();
    Address(const QString &address);
    Address(const QHostAddress &hostAddress, quint16 port);

    void setHostAddress(const QHostAddress &hostAddress);
    QHostAddress hostAddress() const;

    quint16 port() const;
    void setPort(quint16 port);

    void setAddress(const QString &address);
    QString address() const;

private:
    QHostAddress m_hostAddress;
    quint16 m_port;
    QString m_address;
};

class Option
{
public:
    Option();
    Option(Message::OptionType optionType, const QByteArray &data);

    Message::OptionType type() const;
    QByteArray data() const;
    bool isValid() const;

    bool operator ==(const Option &other);
private:
    Message::OptionType m_type;
    QByteArray m_data;
};

} // coap
} // iotlib

QDebug operator<<(QDebug debug, const iotlib::coap::Message &message);

#endif // COAP_MESSAGE_H
