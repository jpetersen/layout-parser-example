#ifndef LAYOUTPARSER_H
#define LAYOUTPARSER_H

#include <QLatin1String>
#include <QXmlStreamReader>

class LayoutParser
{
public:
    explicit LayoutParser(const QLatin1String &data);

    bool parse();

    const QString errorString() const;

private:
    QXmlStreamReader xml;

    void parseRoot();
    void parseLayout();
    void parseSection();
    void parseRow();
    void parseKey();
    void findRootElement();
    void readToEnd();
};

#endif // LAYOUTPARSER_H
