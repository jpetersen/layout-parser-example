#include "layoutparser.h"

#include <QDebug>

namespace {
    bool boolValue(const QStringRef &value, bool defaultValue) {
        if (value.isEmpty())
            return defaultValue;

        if (value == QLatin1String("true") ||
            value == QLatin1String("1"))
            return true;

        if (value == QLatin1String("false") ||
            value == QLatin1String("0"))
            return false;

        return defaultValue;
    }
}

LayoutParser::LayoutParser(const QLatin1String &data)
    : xml(data),
      mKeyboard()
{
}

bool LayoutParser::parse()
{
    findRootElement();

    if (!xml.hasError())
        parseKeyboard();

    readToEnd();

    return !xml.hasError();
}

void LayoutParser::findRootElement()
{
    Q_ASSERT(xml.tokenType() == QXmlStreamReader::NoToken);

    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType type = xml.readNext();
        if (type == QXmlStreamReader::StartElement)
            return;
    }

    xml.raiseError(QLatin1String("Expected '<keyboard>'."));
}

void LayoutParser::parseKeyboard()
{
    Q_ASSERT(xml.isStartElement());

    if (!xml.isStartElement() || xml.name() != QLatin1String("keyboard")) {
        xml.raiseError(QString(QLatin1String("Expected '<keyboard>', but got '<%1>'.")).arg(xml.name().toString()));
    }

    const QXmlStreamAttributes& attributes = xml.attributes();
    const QString& version = attributes.value(QLatin1String("version")).toString();
    const QString& title = attributes.value(QLatin1String("title")).toString();
    const QString& language = attributes.value(QLatin1String("language")).toString();
    const QString& catalog = attributes.value(QLatin1String("catalog")).toString();
    const bool autocapitalization = boolValue(attributes.value(QLatin1String("autocapitalization")), true);

    mKeyboard = QSharedPointer<Keyboard>(new Keyboard(version, title, language, catalog, autocapitalization));

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("layout")) {
            parseLayout();
        } else if (xml.name() == QLatin1String("import")) {

        } else {
            xml.raiseError(QString(QLatin1String("Expected '<layout>' or '<import>', but got '<%1>'.")).arg(xml.name().toString()));
        }
    }
}

void LayoutParser::parseLayout()
{
    Q_ASSERT(xml.isStartElement());
    Q_ASSERT(xml.name() == QLatin1String("layout"));

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("section")) {
            parseSection();
        } else {
            xml.raiseError(QString(QLatin1String("Expected '<section>', but got '<%1>'.")).arg(xml.name().toString()));
        }
    }
}

void LayoutParser::parseSection()
{
    Q_ASSERT(xml.isStartElement());
    Q_ASSERT(xml.name() == QLatin1String("section"));

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("row")) {
            parseRow();
        } else {
            xml.raiseError(QString(QLatin1String("Expected '<row>', but got '<%1>'.")).arg(xml.name().toString()));
        }
    }
}

void LayoutParser::parseRow()
{
    Q_ASSERT(xml.isStartElement());
    Q_ASSERT(xml.name() == QLatin1String("row"));

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("key")) {
            parseKey();
        } else {
            xml.raiseError(QString(QLatin1String("Expected '<key>', but got '<%1>'.")).arg(xml.name().toString()));
        }
    }
}

void LayoutParser::parseKey()
{
    Q_ASSERT(xml.isStartElement());
    Q_ASSERT(xml.name() == QLatin1String("key"));

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("binding")) {
            // parseBinding();
            xml.skipCurrentElement();
        } else {
            xml.raiseError(QString(QLatin1String("Expected '<binding>', but got '<%1>'.")).arg(xml.name().toString()));
        }
    }
}

void LayoutParser::readToEnd()
{
    while (!xml.atEnd()) {
        xml.readNext();
    }
}

const QString LayoutParser::errorString() const
{
    return xml.errorString();
}

const QSharedPointer<Keyboard> LayoutParser::keyboard() const
{
    return mKeyboard;
}
