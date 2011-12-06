#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QScopedPointer>

#include "layoutparser.h"

class LayoutParserTest : public QObject
{
    Q_OBJECT

public:
    LayoutParserTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testValidXML_data();
    void testValidXML();
    void testInvalidXML_data();
    void testInvalidXML();
    void testKeyboardAttributes_data();
    void testKeyboardAttributes();
    void testImportAttributes_data();
    void testImportAttributes();
    void testLayoutAttributes_data();
    void testLayoutAttributes();

private:
    void parseAndVerify(const QByteArray &data);
    bool parse(const QByteArray &data);

    QScopedPointer<LayoutParser> subject;
};

Q_DECLARE_METATYPE(QList<QSharedPointer<Layout> >)

LayoutParserTest::LayoutParserTest()
{
}

void LayoutParserTest::initTestCase()
{
}

void LayoutParserTest::cleanupTestCase()
{
    subject.reset();
}

void LayoutParserTest::testValidXML_data()
{
    QTest::addColumn<QByteArray>("document");

    QTest::newRow("root") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard/>");
    QTest::newRow("root with end") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard></keyboard>");
    QTest::newRow("layout") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout><section/></layout></keyboard>");
    QTest::newRow("import") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><import/></keyboard>");
    QTest::newRow("multiple layouts and imports") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout><section/></layout><layout><section/></layout><import/><layout><section/></layout><import/></keyboard>");
    QTest::newRow("full file") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE keyboard SYSTEM 'VirtualKeyboardLayout.dtd'><keyboard title=\"Deutsch\" version=\"1.0\" catalog=\"de\" language=\"de\"><layout type=\"general\"><section id=\"main\"><row><key><binding label=\"q\"/><binding shift=\"true\" label=\"Q\"/></key><key><binding label=\"w\"/><binding shift=\"true\" label=\"W\"/></key></row><row><key><binding label=\"a\" extended_labels=\"äàáãâåæ\"/><binding shift=\"true\" label=\"A\" extended_labels=\"ÄÀÁÃÂÅÆ\"/></key></row></section></layout></keyboard>");
}

void LayoutParserTest::testValidXML()
{
    QFETCH(QByteArray, document);

    parseAndVerify(document);
}

void LayoutParserTest::parseAndVerify(const QByteArray &document)
{
    const bool result = parse(document);
    if (!result)
        qDebug() << subject->errorString();

    QVERIFY(result);
}

bool LayoutParserTest::parse(const QByteArray &document)
{
    QByteArray data(document);
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    subject.reset(new LayoutParser(&buffer));

    const bool result = subject->parse();

    return result;
}

void LayoutParserTest::testInvalidXML_data()
{
    QTest::addColumn<QByteArray>("document");
    QTest::addColumn<QString>("error");

    QTest::newRow("invalid") << QByteArray("<keyboard><bar/></foo>")
                             << "Expected '<layout>' or '<import>', but got '<bar>'.";
    QTest::newRow("tag mismatch") << QByteArray("<keyboard><layout><section/></foo>")
                                  << "Opening and ending tag mismatch.";
    QTest::newRow("no root") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?>")
                             << "Premature end of document.";
    QTest::newRow("no root2") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?></foo>")
                              << "Expected '?', '!', or '[a-zA-Z]', but got '/'.";
    QTest::newRow("extra content") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard/><foo/>")
                                   << "Extra content at end of document.";
    QTest::newRow("wrong root") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><foo>")
                                << "Expected '<keyboard>', but got '<foo>'.";
    QTest::newRow("wrong char") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><foo/</keyboard>")
                                << "Expected '>', but got '<'.";
    QTest::newRow("wrong element") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><foo/></keyboard>")
                                   << "Expected '<layout>' or '<import>', but got '<foo>'.";
    QTest::newRow("wrong element inside key") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE keyboard SYSTEM 'VirtualKeyboardLayout.dtd'><keyboard title=\"Deutsch\" version=\"1.0\" catalog=\"de\" language=\"de\"><layout type=\"general\"><section id=\"main\"><row><key><binding label=\"q\"/><row/></key><key><binding label=\"w\"/><binding shift=\"true\" label=\"W\"/></key></row><row><key><binding label=\"a\" extended_labels=\"äàáãâåæ\"/><binding shift=\"true\" label=\"A\" extended_labels=\"ÄÀÁÃÂÅÆ\"/></key></row></section></layout></keyboard>")
                                              << "Expected '<binding>', but got '<row>'.";
    QTest::newRow("wrong element inside row") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE keyboard SYSTEM 'VirtualKeyboardLayout.dtd'><keyboard title=\"Deutsch\" version=\"1.0\" catalog=\"de\" language=\"de\"><layout type=\"general\"><section id=\"main\"><row><key><binding label=\"q\"/></key></row><row><binding/></row></section></layout></keyboard>")
                                              << "Expected '<key>', but got '<binding>'.";
    QTest::newRow("layout type") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout type=\"foo\"><section/></layout></keyboard>")
                                 << "Expected one of 'general', 'url', 'email', 'number', 'phonenumber', 'common', but got 'foo'.";
    QTest::newRow("layout orientation") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout orientation=\"foo\"><section/></layout></keyboard>")
                                        << "Expected one of 'landscape', 'portrait', but got 'foo'.";
    QTest::newRow("layout type and orientation") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout type=\"foo\" orientation=\"foo\"><section/></layout></keyboard>")
                                                 << "Expected one of 'general', 'url', 'email', 'number', 'phonenumber', 'common', but got 'foo'.";
    QTest::newRow("layout without section") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout/></keyboard>")
                                            << "Expected '<section>'.";

}

void LayoutParserTest::testInvalidXML()
{
    QFETCH(QByteArray, document);
    QFETCH(QString, error);

    QVERIFY(!parse(document));
    QCOMPARE(subject->errorString(), QLatin1String(error.toLatin1().constData()));
}

void LayoutParserTest::testKeyboardAttributes_data()
{
    QTest::addColumn<QByteArray>("document");
    QTest::addColumn<QString>("version");
    QTest::addColumn<QString>("title");
    QTest::addColumn<QString>("language");
    QTest::addColumn<QString>("catalog");
    QTest::addColumn<bool>("autocapitalization");

    QTest::newRow("default") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard/>")
                             << QString::fromLatin1("1.0")
                             << QString()
                             << QString()
                             << QString()
                             << true;
    QTest::newRow("version") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard version=\"3.4\"/>")
                             << QString::fromLatin1("3.4")
                             << QString()
                             << QString()
                             << QString()
                             << true;
    QTest::newRow("title") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard title=\"ATitle\"/>")
                           << QString::fromLatin1("1.0")
                           << QString::fromLatin1("ATitle")
                           << QString()
                           << QString()
                           << true;
    QTest::newRow("language") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard language=\"ALanguage\"/>")
                              << QString::fromLatin1("1.0")
                              << QString()
                              << QString::fromLatin1("ALanguage")
                              << QString()
                              << true;
    QTest::newRow("all") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard version=\"3.4\" title=\"ATitle\" language=\"ALanguage\" catalog=\"ACatalog\" autocapitalization=\"false\"/>")
                         << QString::fromLatin1("3.4")
                         << QString::fromLatin1("ATitle")
                         << QString::fromLatin1("ALanguage")
                         << QString::fromLatin1("ACatalog")
                         << false;
}

void LayoutParserTest::testKeyboardAttributes()
{
    QFETCH(QByteArray, document);
    QFETCH(QString, version);
    QFETCH(QString, title);
    QFETCH(QString, language);
    QFETCH(QString, catalog);
    QFETCH(bool, autocapitalization);

    parseAndVerify(document);

    QCOMPARE(version, subject->keyboard()->version());
    QCOMPARE(title, subject->keyboard()->title());
    QCOMPARE(language, subject->keyboard()->language());
    QCOMPARE(catalog, subject->keyboard()->catalog());
    QCOMPARE(autocapitalization, subject->keyboard()->autocapitalization());
}

void LayoutParserTest::testImportAttributes_data()
{
    QTest::addColumn<QByteArray>("document");
    QTest::addColumn<QStringList>("files");

    QTest::newRow("empty") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard></keyboard>")
                           << QStringList();
    QTest::newRow("one") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><import file=\"de-general.xml\"/></keyboard>")
                         << QStringList(QString::fromLatin1("de-general.xml"));
    QStringList files;
    files << QString::fromLatin1("de-general.xml") << QString::fromLatin1("de-special.xml");
    QTest::newRow("two") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><import file=\"de-general.xml\"/><import file=\"de-special.xml\"/></keyboard>")
                         << files;
    files.clear();
    files << QString::fromLatin1("de-general.xml") << QString::fromLatin1("de-general2.xml")
          << QString::fromLatin1("de-special.xml") << QString::fromLatin1("de-special2.xml");
    QTest::newRow("mixed") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><import file=\"de-general.xml\"/><layout><section/></layout><import file=\"de-general2.xml\"/><layout><section/></layout><import file=\"de-special.xml\"/><import file=\"de-special2.xml\"/></keyboard>")
                           << files;
}

void LayoutParserTest::testImportAttributes()
{
    QFETCH(QByteArray, document);
    QFETCH(QStringList, files);

    parseAndVerify(document);

    QCOMPARE(subject->imports(), files);
}

void LayoutParserTest::testLayoutAttributes_data()
{
    QTest::addColumn<QByteArray>("document");
    QTest::addColumn<QList<QSharedPointer<Layout> > >("layouts");

    QList<QSharedPointer<Layout> > layouts;

    layouts.clear();
    layouts << QSharedPointer<Layout>(new Layout(Layout::General, Layout::Landscape));
    QTest::newRow("default") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout><section/></layout></keyboard>")
                             << layouts;
    layouts.clear();
    layouts << QSharedPointer<Layout>(new Layout(Layout::General, Layout::Landscape));
    QTest::newRow("explicit default") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout type=\"general\" orientation=\"landscape\"><section/></layout></keyboard>")
                                      << layouts;
    layouts.clear();
    layouts << QSharedPointer<Layout>(new Layout(Layout::Email, Layout::Portrait))
            << QSharedPointer<Layout>(new Layout(Layout::General, Layout::Landscape))
            << QSharedPointer<Layout>(new Layout(Layout::Common, Layout::Landscape));
    QTest::newRow("mixed") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout type=\"email\" orientation=\"portrait\"><section/></layout><layout><section/></layout><layout type=\"common\"><section/></layout></keyboard>")
                           << layouts;
}

template<>
bool operator==(const QSharedPointer<Layout>& layout, const QSharedPointer<Layout>& other)
{
    // Compare layouts themselves instead of the pointers
    return *layout == *other;
}

void LayoutParserTest::testLayoutAttributes()
{
    QFETCH(QByteArray, document);
    QFETCH(QList<QSharedPointer<Layout> >, layouts);

    parseAndVerify(document);

    QCOMPARE(subject->layouts().size(), layouts.size());
    QCOMPARE(subject->layouts(), layouts);
}

QTEST_MAIN(LayoutParserTest);

#include "tst_layoutparsertest.moc"
