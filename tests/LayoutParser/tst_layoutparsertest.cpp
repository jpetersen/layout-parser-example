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

    QByteArray data(document.toUtf8());
    QScopedPointer<QBuffer> buffer(new QBuffer(&data));
    buffer->open(QIODevice::ReadOnly);
    LayoutParser parser(buffer.data());

    bool result = parser.parse();
    if (!result)
        qDebug() << parser.errorString();

    QVERIFY(result);
}

void LayoutParserTest::testInvalidXML_data()
{
    QTest::addColumn<QByteArray>("document");
    QTest::addColumn<QString>("error");

    QTest::newRow("invalid") << QByteArray("<keyboard><bar/></foo>")
                             << "Expected '<layout>' or '<import>', but got '<bar>'.";
    QTest::newRow("tag mismatch") << QByteArray("<keyboard><layout/></foo>")
                                  << "Opening and ending tag mismatch.";
    QTest::newRow("no root") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?>")
                             << "Expected '<keyboard>'.";
    QTest::newRow("no root2") << QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?></foo>")
                              << "Expected '<keyboard>'.";
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
}

void LayoutParserTest::testInvalidXML()
{
    QFETCH(QByteArray, document);
    QFETCH(QString, error);

    QScopedPointer<QBuffer> buffer(new QBuffer(&document));
    buffer->open(QIODevice::ReadOnly);
    LayoutParser parser(buffer.data());

    QVERIFY(!parser.parse());
    QCOMPARE(parser.errorString(), QLatin1String(error.toLatin1().constData()));
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

    QScopedPointer<QBuffer> buffer(new QBuffer(&document));
    buffer->open(QIODevice::ReadOnly);
    LayoutParser parser(buffer.data());

    bool result = parser.parse();
    if (!result)
        qDebug() << parser.errorString();

    QCOMPARE(version, parser.keyboard()->version());
    QCOMPARE(title, parser.keyboard()->title());
    QCOMPARE(language, parser.keyboard()->language());
    QCOMPARE(catalog, parser.keyboard()->catalog());
    QCOMPARE(autocapitalization, parser.keyboard()->autocapitalization());
}

QTEST_MAIN(LayoutParserTest);

#include "tst_layoutparsertest.moc"
