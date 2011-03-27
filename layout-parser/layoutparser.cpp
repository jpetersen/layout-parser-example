#include "layoutparser.h"

#include <QDebug>

LayoutParser::LayoutParser(const QLatin1String &data)
    : xml(data)
{
}

bool LayoutParser::parse()
{
    findRootElement();

    if (!xml.hasError())
        parseRoot();

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

void LayoutParser::parseRoot()
{
    Q_ASSERT(xml.isStartElement());

    if (!xml.isStartElement() || xml.name() != QLatin1String("keyboard")) {
        xml.raiseError(QString(QLatin1String("Expected '<keyboard>', but got '<%1>'.")).arg(xml.name().toString()));
    }

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("layout")) {

        } else if (xml.name() == QLatin1String("import")) {

        } else {
            xml.raiseError(QString(QLatin1String("Expected '<layout>' or '<import>', but got '<%1>'.")).arg(xml.name().toString()));
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

