#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

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
    QTest::addColumn<QString>("document");

    QTest::newRow("root") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard/>";
    QTest::newRow("root with end") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard></keyboard>";
    QTest::newRow("layout") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout/></keyboard>";
    QTest::newRow("import") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><import/></keyboard>";
    QTest::newRow("multiple layouts and imports") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><keyboard><layout/><layout/><import/><layout/><import/></keyboard>";
    QTest::newRow("full file") << "<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE keyboard SYSTEM 'VirtualKeyboardLayout.dtd'><keyboard title=\"Deutsch\" version=\"1.0\" catalog=\"de\" language=\"de\"><layout type=\"general\"><section id=\"main\"><row><key><binding label=\"q\"/><binding shift=\"true\" label=\"Q\"/></key><key><binding label=\"w\"/><binding shift=\"true\" label=\"W\"/></key></row><row><key><binding label=\"a\" extended_labels=\"äàáãâåæ\"/><binding shift=\"true\" label=\"A\" extended_labels=\"ÄÀÁÃÂÅÆ\"/></key></row></section></layout></keyboard>";
}

void LayoutParserTest::testValidXML()
{
    QFETCH(QString, document);

    LayoutParser parser(QLatin1String(document.toLatin1().constData()));

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
}

void LayoutParserTest::testInvalidXML()
{
    QFETCH(QString, document);
    QFETCH(QString, error);

    LayoutParser parser(QLatin1String(document.toLatin1().constData()));

    QVERIFY(!parser.parse());
    QCOMPARE(parser.errorString(), QLatin1String(error.toLatin1().constData()));
}

QTEST_MAIN(LayoutParserTest);

#include "tst_layoutparsertest.moc"
