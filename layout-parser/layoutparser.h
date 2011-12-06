#ifndef LAYOUTPARSER_H
#define LAYOUTPARSER_H

c
class LayoutParser
{
public:
    explicit LayoutParser(QIODevice *device);

    bool parse();

    const QString errorString() const;

    const QSharedPointer<Keyboard> keyboard() const;
    const QStringList imports() const;
    const QList<QSharedPointer<Layout> > layouts() const;

private:
    QXmlStreamReader xml;
    QSharedPointer<Keyboard> mKeyboard;
    QStringList mImports;
    QList<QSharedPointer<Layout> > mLayouts;

    void parseKeyboard();
    void parseImport();
    void parseLayout();
    void parseSection();
    void parseRow();
    void parseKey();
    void findRootElement();
    void readToEnd();

    void error(const QString &message);

    bool boolValue(const QStringRef &value, bool defaultValue);

    template <class E>
    E enumValue(const char * const attribute, const QStringList &values, E defaultValue);
};

#endif // LAYOUTPARSER_H
