#ifndef LAYOUTPARSER_H
#define LAYOUTPARSER_H

#include <QIODevice>
#include <QXmlStreamReader>
#include <QSharedPointer>

#include "keyboard.h"

class LayoutParser
{
public:
    explicit LayoutParser(QIODevice *device);

    bool parse();

    const QString errorString() const;

    const QSharedPointer<Keyboard> keyboard() const;

private:
    QXmlStreamReader xml;
    QSharedPointer<Keyboard> mKeyboard;

    void parseKeyboard();
    void parseLayout();
    void parseSection();
    void parseRow();
    void parseKey();
    void findRootElement();
    void readToEnd();
};

#endif // LAYOUTPARSER_H
