#include "message.hpp"
#include "endianhelper.h"

#include <QAtomicInt>
#include <QDebug>
#include <QMetaEnum>

namespace iotlib {
namespace coap {

Option::Option()
    : m_type((Message::OptionType)0)
{ }

Option::Option(Message::OptionType optionType, const QByteArray &data) :
    m_type(optionType), m_data(data)
{ }

Message::OptionType Option::type() const
{
    return m_type;
}

QByteArray Option::data() const
{
    return m_data;
}

bool Option::isValid() const
{
    return (quint16)m_type != 0;
}

bool Option::operator ==(const Option &other)
{
    return (m_type == other.m_type) && (m_data == other.m_data);
}



class MessagePrivate : public QSharedData
{
public:
    MessagePrivate() :
        version(1),
        type(Message::Type::Reset),
        code(Message::Code::Empty),
        message_id(0)
    {
        // TODO   d->errors =
    }
    MessagePrivate(const MessagePrivate &other) :
        QSharedData(other),
        version(other.version),
        type(other.type),
        code(other.code),
        message_id(other.message_id),
        token(other.token),
        options(other.options),
        payload(other.payload),
        address(other.address),
        port(other.port),
        errors(other.errors)
    { }
    ~MessagePrivate() { }

    quint8 version;
    Message::Type type;
    Message::Code code;
    quint16 message_id;
    QByteArray token;
    QList<Option> options;
    QByteArray payload;
    QHostAddress address;
    quint16 port;

    enum Error {
        FORMAT_ERROR           = 1,
        UNKNOWN_VERSION        = 2,
        WRONG_TOKEN_LENGTH     = 4,
        WRONG_PAYLOAD_MARKER   = 8,
        WRONG_OPTION_HEADER    = 16,
        NOT_ENOUGH_DATA        = 32,
        WRONG_VERSION          = 64,
        WRONG_TOKEN            = 128,
    };
    Q_DECLARE_FLAGS(Errors, Error)
    Errors errors;
};

Address::Address()
{
}

Address::Address(const QString &address) : m_address(address)
{
}

Address::Address(const QHostAddress &hostAddress, quint16 port) :
    m_hostAddress(hostAddress), m_port(port)
{
}

Address Address::setHostAddress(const QHostAddress &hostAddress)
{
    m_hostAddress = hostAddress;
    return *this;
}

QHostAddress Address::hostAddress() const
{
    return m_hostAddress;
}

Address Address::setPort(quint16 port)
{
    m_port = port;
    return *this;
}

quint16 Address::port() const
{
    return m_port;
}

Address Address::setAddress(const QString &address)
{
    m_address = address;
    m_hostAddress = QHostAddress(address);
    return *this;
}

QString Address::address() const
{
    if (m_address.isEmpty())
        return m_hostAddress.toString();
    return m_address;
}

Message::Message()
    : d(new MessagePrivate)
{

}

Message::Message(const Message &other) :
    d(other.d)
{

}

Message &Message::operator=(const Message &other)
{
    if (this == &other) // protect against self-assignment
        return *this;
    d = other.d;
    return *this;
}

Message::~Message()
{
}

void Message::setVersion(quint8 version)
{
    d->version = version;
    if (version != 1)
        d->errors |= MessagePrivate::WRONG_VERSION;
}

quint8 Message::version() const
{
    return d->version;
}

void Message::setType(Message::Type type)
{

    if (d->type == type)
        return;
    d->type = type;
}

Message::Type Message::type() const
{
    return d->type;
}

void Message::setCode(Message::Code code)
{
    if (d->code == code)
        return;
    d->code = code;
}

Message::Code Message::code() const
{
    return d->code;
}

bool Message::isEmpty() const
{
    return d->code == Code::Empty;
}

bool Message::isRequest() const
{
    return ((quint8)d->code >= 0x01 && (quint8)d->code <= 0x04);
}

bool Message::isResponse() const
{
    return (!isEmpty() && !isRequest());
}

void Message::setToken(const QByteArray &token)
{
    if (d->token == token)
        return;
    d->token = token;
    if (token.length() > 8)
        d->errors |= MessagePrivate::WRONG_TOKEN;
}

void Message::setToken(const char *token, quint8 length)
{
    QByteArray t(token, length);
    if (d->token == t)
        return;
    d->token = t;
    if (length > 8)
        d->errors |= MessagePrivate::WRONG_TOKEN;
}

QByteArray Message::token() const
{
    return d->token;
}

void Message::setMessageId(quint16 id)
{
    if (d->message_id == id)
        return;
    d->message_id = id;
}

quint16 Message::messageId() const
{
    return d->message_id;
}

void Message::addOption(Message::OptionType optionType, const QByteArray &value)
{
    int idx = 0;
    for (int i = 0; i < d->options.length(); ++i) {
        if (optionType > d->options[i].type())
            idx++;
    }
    Option option(optionType, value);
    if (d->options.contains(option))
        return;
    d->options.insert(idx, option);
}

QList<Option> Message::options() const
{
    return d->options;
}

int Message::optionsCount() const
{
    return d->options.length();
}

Option Message::option(int idx) const
{
    if (idx < 0 || idx >= d->options.length())
        return Option();
    return d->options[idx];
}

void Message::setUrl(const QUrl &url)
{
    foreach (const QString &path, url.path().split("/", QString::SkipEmptyParts))
        addOption(OptionType::UriPath, path.toUtf8());
}

void Message::setContentFormat(Message::ContentFormat format)
{
    Message::OptionType optionContentFormat = OptionType::ContentFormat;
    if (format == ContentFormat::TextPlain) {
        addOption(optionContentFormat, QByteArray());
    } else if ((quint16)format < 256) {
        quint8 f = (quint8)format;
        addOption(optionContentFormat, QByteArray((const char*)&f, 1));
    } else {
        quint16 f = (quint16)format;
        QByteArray data;
        data.resize(2);
        endian_store16(data.data(), f);
        addOption(optionContentFormat, data);
    }
}

Message::ContentFormat Message::contentFormat() const
{
    foreach (const Option &option, d->options)
        if (option.type() == OptionType::ContentFormat)
            return (ContentFormat)option.data().data()[0];
    return ContentFormat::TextPlain;
}

void Message::setContent(const QByteArray &content)
{
    d->payload = content;
}

QByteArray Message::content() const
{
    return d->payload;
}

quint8 *pack_option(quint8 *p, quint16 optionNumber, const QByteArray &value, bool write)
{
    quint8 *h = p;
    p++;
    if (optionNumber <= 12) {
        if (write)
            *h = (quint8)optionNumber << 4;
    } else if (optionNumber > 12 && optionNumber <= 255) {
        if (write) {
            *h = 13 << 4;
            *p = (quint8)optionNumber - 13;
        }
        p++;
    } else if (optionNumber > 255) {
        *h = 14 << 4;
        // write me!!!

    }

    if (value.length() <= 12) {
        if (write)
            *h |= (quint8)value.length();
    } else if (value.length() > 12 && value.length() <= 255) {
        if (write) {
            *h |= 13;
            *p = (quint8)value.length();
        }
        p++;
    } else if (value.length() > 255) {
        *h |= 14;
        //write me
    }

    if (write)
        memcpy(p, value.data(), value.length());
    p += value.length();

    return p;
}

QByteArray Message::pack() const
{

//    const_cast<MessagePrivate *>(d)->errors = Errors(0);
    quint32 pduSize = 4; // header
    pduSize += d->token.length();
    quint8 *p = 0;
    quint16 optionDelta = 0;
    for (int i = 0; i < d->options.length(); ++i) {
        quint16 optionNumber = (quint16)d->options[i].type() - optionDelta;
        p = pack_option(p, optionNumber, d->options[i].data(), false);
        optionDelta = optionNumber;
    }
    pduSize += p - (quint8 *)0;

    if (d->payload.size() > 0)
        pduSize += 1; // payload marker
    pduSize += d->payload.length();

    QByteArray packed;
    packed.resize(pduSize);
    p = (quint8 *)packed.data();
    p[0] = d->version << 6;
    p[0] |= (quint8)d->type << 4;
    p[0] |= (quint8)d->token.length();
    p[1] = (quint8)d->code;
    endian_store16((p + 2), d->message_id);
    p += 4;
    memcpy(p, d->token.data(), d->token.length());
    p += d->token.length();
    optionDelta = 0;
    for (int i = 0; i < d->options.length(); ++i) {
        quint16 optionNumber = (quint16)d->options[i].type() - optionDelta;
        p = pack_option(p, optionNumber, d->options[i].data(), true);
        optionDelta = optionNumber;
    }
    *(p++) = 0xff;
    memcpy(p, d->payload, d->payload.length());

    return packed;
}

void Message::unpack(const QByteArray &packed)
{
    d->errors = MessagePrivate::Errors(0);
    d->options.clear();
    quint8 *p = (quint8 *)packed.data();
    quint8 *pend = (quint8 *)packed.data() + packed.size() - 1;

    if (packed.size() < 4) {
        d->errors |= MessagePrivate::FORMAT_ERROR;
        return;
    }

    d->version = (p[0] & 0xc0) >> 6;
    if (d->version != 1)
        d->errors |= MessagePrivate::UNKNOWN_VERSION;

    d->type = (Message::Type)( (p[0] & 0x30) >> 4 );
    quint8 tokenLength = (p[0] & 0xf);
    d->code = (Message::Code)p[1];
    if (tokenLength > 8)
        d->errors |= MessagePrivate::WRONG_TOKEN_LENGTH;
    else
        d->token = QByteArray((const char *)p + 4, tokenLength);

    d->message_id = endian_load16(quint16, (p + 2));

    if (packed.size() == 4) // only header present, ok
        return;

    p += tokenLength;
    p += 4;

    quint16 optionNumber = 0;
    do {
        if (*p == 0xff) {
            quint32 payloadSize = pend - p;
            if (payloadSize == 0)
                d->errors |= MessagePrivate::WRONG_PAYLOAD_MARKER;
            d->payload = QByteArray((const char *)p + 1, payloadSize);
            return;
        }

        quint16 optionDelta = (*p & 0xf0) >> 4;
        quint16 optionLength = (*p & 0xf);
        p++;
        if (optionDelta == 13) {
            optionDelta = *p + 13;
            p++;
        } else if (optionDelta == 14) {
            optionDelta = endian_load16(quint16, p);
            p += 2;
        } else if (optionDelta == 15) {
            d->errors |= MessagePrivate::WRONG_PAYLOAD_MARKER;
            return;
        }
        if (optionLength == 13) {
            optionLength = *p + 13;
            p++;
        } else if (optionLength == 14) {
            optionLength = endian_load16(quint16, p);
            p += 2;
        } else if (optionLength == 15) {
            d->errors |= MessagePrivate::WRONG_PAYLOAD_MARKER;
            return;
        }

        optionNumber += optionDelta;
        Option option((Message::OptionType)optionNumber, QByteArray((const char *)p, optionLength));
        d->options.append(option);
        p += optionLength;
    } while (p <= pend);

    if (p > pend + 1)
        d->errors |= MessagePrivate::NOT_ENOUGH_DATA;
}

Address Message::address() const
{

}

void Message::setAddress(const Address &address)
{

}

bool Message::isValid() const
{
    return d->errors == MessagePrivate::Errors(0);
}

} // iotlib
} // coap

QDebug operator<<(QDebug debug, const iotlib::coap::Message &pdu)
{
    QDebugStateSaver saver(debug);
    debug.nospace();
    if (!pdu.isValid()) {
        debug << "Message(Invalid)";
        return debug;
    }
    int typeEnum = pdu.staticMetaObject.indexOfEnumerator("Type");
    int codeEnum = pdu.staticMetaObject.indexOfEnumerator("Code");
    int optionTypeEnum = pdu.staticMetaObject.indexOfEnumerator("OptionType");
    int contentFormatEnum = pdu.staticMetaObject.indexOfEnumerator("ContentFormat");
    debug << "Message(" << pdu.staticMetaObject.enumerator(typeEnum).key((int)pdu.type());
    debug << pdu.staticMetaObject.enumerator(codeEnum).key((int)pdu.code()) << " ";
    debug << pdu.staticMetaObject.enumerator(1).key(123) << " ";
    debug << "Token:" << pdu.token().toHex() << " MID:" << pdu.messageId() << " ";

//    for (int i = 0; i < pdu.optionsCount(); ++i) {
//        QDebug dbg(QtDebugMsg);
//        dbg.nospace();
//        dbg << "Option:" << i << ", ";
//        Option option = pdu.option(i);
//        dbg << "isValid:" << option.isValid() << ", ";
//        if (!option.isValid())
//            continue;
//        dbg << "Number:" << (int)option.type() << " - " << Coap::toString(option.type()) << ", ";
//        dbg << "Data:" << option.data().toHex();
//        //        if (option.type() == Coap::OptionType:: || option.type() == 3)
//        dbg << " asString: " << option.data();
//    }
    if (pdu.content().isEmpty())
        debug << "No payload ";
    else
        debug << "Payload:" << pdu.content().toHex() << " asString:" << pdu.content();

    debug << ")";
    return debug;
}
