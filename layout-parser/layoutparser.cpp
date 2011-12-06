#include "layoutparser.h"

#include <QDebug>

LayoutParser::LayoutParser(QIODevice *device)
    : xml(device),
      mKeyboard(),
      mImports(),
      mLayouts()
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

    Q_ASSERT(xml.hasError());
}

void LayoutParser::error(const QString &message)
{
    if (xml.hasError())
        return;

    xml.raiseError(message);
}

void LayoutParser::parseKeyboard()
{
    Q_ASSERT(xml.isStartElement());

    if (!xml.isStartElement() || xml.name() != QLatin1String("keyboard")) {
        error(QString::fromLatin1("Expected '<keyboard>', but got '<%1>'.").arg(xml.name().toString()));
    }

    const QXmlStreamAttributes& attributes = xml.attributes();
    const QString& version = attributes.value(QLatin1String("version")).toString();
    const QString& title = attributes.value(QLatin1String("title")).toString();
    const QString& language = attributes.value(QLatin1String("language")).toString();
    const QString& catalog = attributes.value(QLatin1String("catalog")).toString();
    const bool autocapitalization = boolValue(attributes.value(QLatin1String("autocapitalization")), true);

    mKeyboard = QSharedPointer<Keyboard>(new Keyboard(version, title, language, catalog, autocapitalization));

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("import")) {
            parseImport();
        } else if (xml.name() == QLatin1String("layout")) {
            parseLayout();
        } else {
            error(QString::fromLatin1("Expected '<layout>' or '<import>', but got '<%1>'.").arg(xml.name().toString()));
        }
    }
}

bool LayoutParser::boolValue(const QStringRef &value, bool defaultValue) {
    if (value.isEmpty())
        return defaultValue;

    if (value == QLatin1String("true") ||
        value == QLatin1String("1"))
        return true;

    if (value == QLatin1String("false") ||
        value == QLatin1String("0"))
        return false;

    error(QString::fromLatin1("Excpected 'true', 'false', '1' or '0', but got '%1'.").arg(value.toString()));

    return defaultValue;
}

void LayoutParser::parseImport()
{
    Q_ASSERT(xml.isStartElement());
    Q_ASSERT(xml.name() == QLatin1String("import"));

    const QXmlStreamAttributes& attributes = xml.attributes();
    const QString& file = attributes.value(QLatin1String("file")).toString();
    if (!file.isEmpty()) {
        mImports.append(file);
    }

    xml.skipCurrentElement();
}

void LayoutParser::parseLayout()
{
    Q_ASSERT(xml.isStartElement());
    Q_ASSERT(xml.name() == QLatin1String("layout"));

    static const QStringList typeValues(QString::fromLatin1("general,url,email,number,phonenumber,common").split(','));
    static const QStringList orientationValues(QString::fromLatin1("landscape,portrait").split(','));

    const Layout::LayoutType type = enumValue("type", typeValues, Layout::General);
    const Layout::LayoutOrientation orientation = enumValue("orientation", orientationValues, Layout::Landscape);

    mLayouts.append(QSharedPointer<Layout>(new Layout(type, orientation)));

    bool foundSection = false;

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("section")) {
            foundSection = true;
            parseSection();
        } else {
            error(QString::fromLatin1("Expected '<section>', but got '<%1>'.").arg(xml.name().toString()));
        }
    }

    if (!foundSection)
        error(QString::fromLatin1("Expected '<section>'."));
}

template <class E>
E LayoutParser::enumValue(const char * const attribute, const QStringList &values, E defaultValue)
{
    if (xml.hasError())
        return defaultValue;

    const QXmlStreamAttributes& attributes = xml.attributes();
    const QStringRef& value = attributes.value(QLatin1String(attribute));

    if (value.isEmpty())
        return defaultValue;

    const int index = values.indexOf(value.toString());

    if (index == -1) {
        error(QString::fromLatin1("Expected one of '%1', but got '%2'.").arg(values.join("', '"), value.toString()));

        return defaultValue;
    }

    return static_cast<E>(index);
}

void LayoutParser::parseSection()
{
    Q_ASSERT(xml.isStartElement());
    Q_ASSERT(xml.name() == QLatin1String("section"));

    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("row")) {
            parseRow();
        } else {
            error(QString::fromLatin1("Expected '<row>', but got '<%1>'.").arg(xml.name().toString()));
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
            error(QString::fromLatin1("Expected '<key>', but got '<%1>'.").arg(xml.name().toString()));
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
            error(QString::fromLatin1("Expected '<binding>', but got '<%1>'.").arg(xml.name().toString()));
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

const QStringList LayoutParser::imports() const
{
    return mImports;
}

const QList<QSharedPointer<Layout> > LayoutParser::layouts() const
{
    return mLayouts;
}
