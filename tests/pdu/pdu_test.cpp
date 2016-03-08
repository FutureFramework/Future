#include <QtTest>

class PDUTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_pack();

};

void PDUTest::test_pack()
{

}

QTEST_APPLESS_MAIN(PDUTest)

#include "pdu_test.moc"