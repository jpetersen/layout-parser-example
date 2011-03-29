#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include "layoutparser.h"

Q_DECLARE_METATYPE(std::string)

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
};

LayoutParserTest::LayoutParserTest()
{
}

void LayoutParserTest::initTestCase()
{
}

void LayoutParserTest::cleanupTestCase()
{
}

void LayoutParserTest::testValidXML_data()
{
    QTest::addColumn<std::string>("document");

    QTest::newRow("root") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard/>");
    QTest::newRow("root with end") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard></keyboard>");
    QTest::newRow("layout") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout/></keyboard>");
    QTest::newRow("import") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><import/></keyboard>");
    QTest::newRow("multiple layouts and imports") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout/><layout/><import/><layout/><import/></keyboard>");
    QTest::newRow("full file") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE keyboard SYSTEM 'VirtualKeyboardLayout.dtd'><keyboard title=\"Deutsch\" version=\"1.0\" catalog=\"de\" language=\"de\"><layout type=\"general\"><section id=\"main\"><row><key><binding label=\"q\"/><binding shift=\"true\" label=\"Q\"/></key><key><binding label=\"w\"/><binding shift=\"true\" label=\"W\"/></key></row><row><key><binding label=\"a\" extended_labels=\"äàáãâåæ\"/><binding shift=\"true\" label=\"A\" extended_labels=\"ÄÀÁÃÂÅÆ\"/></key></row></section></layout></keyboard>");
}

void LayoutParserTest::testValidXML()
{
    QFETCH(std::string, document);

    LayoutParser parser(QLatin1String(document.c_str()));

    bool result = parser.parse();
    if (!result)
        qDebug() << parser.errorString();

    QVERIFY(result);
}

void LayoutParserTest::testInvalidXML_data()
{
    QTest::addColumn<QString>("document");
    QTest::addColumn<QString>("error");

    QTest::newRow("invalid") << "<keyboard><bar/></foo>"
                             << "Expected '<layout>' or '<import>', but got '<bar>'.";
    QTest::newRow("tag mismatch") << "<keyboard><layout/></foo>"
                                  << "Opening and ending tag mismatch.";
    QTest::newRow("no root") << "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                             << "Expected '<keyboard>'.";
    QTest::newRow("no root2") << "<?xml version=\"1.0\" encoding=\"utf-8\"?></foo>"
                              << "Expected '<keyboard>'.";
    QTest::newRow("extra content") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard/><foo/>"
                                   << "Extra content at end of document.";
    QTest::newRow("wrong root") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><foo>"
                                << "Expected '<keyboard>', but got '<foo>'.";
    QTest::newRow("wrong char") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><foo/</keyboard>"
                                << "Expected '>', but got '<'.";
    QTest::newRow("wrong element") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><foo/></keyboard>"
                                   << "Expected '<layout>' or '<import>', but got '<foo>'.";
    QTest::newRow("wrong element inside key") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE keyboard SYSTEM 'VirtualKeyboardLayout.dtd'><keyboard title=\"Deutsch\" version=\"1.0\" catalog=\"de\" language=\"de\"><layout type=\"general\"><section id=\"main\"><row><key><binding label=\"q\"/><row/></key><key><binding label=\"w\"/><binding shift=\"true\" label=\"W\"/></key></row><row><key><binding label=\"a\" extended_labels=\"äàáãâåæ\"/><binding shift=\"true\" label=\"A\" extended_labels=\"ÄÀÁÃÂÅÆ\"/></key></row></section></layout></keyboard>"
                                              << "Expected '<binding>', but got '<row>'.";
    QTest::newRow("wrong element inside row") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE keyboard SYSTEM 'VirtualKeyboardLayout.dtd'><keyboard title=\"Deutsch\" version=\"1.0\" catalog=\"de\" language=\"de\"><layout type=\"general\"><section id=\"main\"><row><key><binding label=\"q\"/></key></row><row><binding/></row></section></layout></keyboard>"
                                              << "Expected '<key>', but got '<binding>'.";
}

void LayoutParserTest::testInvalidXML()
{
    QFETCH(QString, document);
    QFETCH(QString, error);

    LayoutParser parser(QLatin1String(document.toLatin1().constData()));

    QVERIFY(!parser.parse());
    QCOMPARE(parser.errorString(), QLatin1String(error.toLatin1().constData()));
}

void LayoutParserTest::testKeyboardAttributes_data()
{
    QTest::addColumn<std::string>("document");
    QTest::addColumn<QString>("version");
    QTest::addColumn<QString>("title");
    QTest::addColumn<QString>("language");

    QTest::newRow("default") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard/>")
                             << QString::fromLatin1("1.0")
                             << QString()
                             << QString();
    QTest::newRow("version") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard version=\"3.4\"/>")
                             << QString::fromLatin1("3.4")
                             << QString()
                             << QString();
    QTest::newRow("title") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard title=\"ATitle\"/>")
                           << QString::fromLatin1("1.0")
                           << QString::fromLatin1("ATitle")
                           << QString();
    QTest::newRow("language") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard language=\"ALanguage\"/>")
                              << QString::fromLatin1("1.0")
                              << QString()
                              << QString::fromLatin1("ALanguage");
    QTest::newRow("all") << std::string("<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard version=\"3.4\" title=\"ATitle\" language=\"ALanguage\"/>")
                         << QString::fromLatin1("3.4")
                         << QString::fromLatin1("ATitle")
                         << QString::fromLatin1("ALanguage");
}

void LayoutParserTest::testKeyboardAttributes()
{
    QFETCH(std::string, document);
    QFETCH(QString, version);

    LayoutParser parser(QLatin1String(document.c_str()));

    bool result = parser.parse();
    if (!result)
        qDebug() << parser.errorString();

    QCOMPARE(version, parser.keyboard()->version());
}

QTEST_MAIN(LayoutParserTest);

#include "tst_layoutparsertest.moc"
