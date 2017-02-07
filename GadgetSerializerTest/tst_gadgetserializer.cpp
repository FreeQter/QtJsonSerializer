#include "brokentestgadget.h"
#include "testgadget.h"

#include <QJsonSerializer>
#include <QString>
#include <QtTest>

class GadgetSerializerTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testVariantConversions_data();
	void testVariantConversions();
	void testSerialization_data();
	void testSerialization();
	void testInvalidSerialization();
	void testDeserialization_data();
	void testDeserialization();
	void testInvalidDeserialization();

private:
	QJsonSerializer *serializer;

	void generateValidTestData();
};

void GadgetSerializerTest::initTestCase()
{
	QJsonSerializer::registerListConverters<QList<int>>();
	QJsonSerializer::registerListConverters<TestGadget>();
	QJsonSerializer::registerListConverters<QList<TestGadget>>();
	serializer = new QJsonSerializer(this);
}

void GadgetSerializerTest::cleanupTestCase()
{
	serializer->deleteLater();
	serializer = nullptr;
}

void GadgetSerializerTest::testVariantConversions_data()
{
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<int>("targetType");

	QTest::newRow("QList<int>") << QVariant::fromValue<QList<int>>({3, 7, 13})
								<< (int)QVariant::List;
	QList<int> l1 = {0, 1, 2};
	QList<int> l2 = {3, 4, 5};
	QList<int> l3 = {6, 7, 8};
	QTest::newRow("QList<QList<int>>") << QVariant::fromValue<QList<QList<int>>>({l1, l2, l3})
									   << (int)QVariant::List;

	QTest::newRow("QList<TestGadget>") << QVariant::fromValue<QList<TestGadget>>({
																					   TestGadget(),
																					   TestGadget(),
																					   TestGadget()
																				   })
										<< (int)QVariant::List;
	QList<TestGadget> o1 = {
		TestGadget(),
		TestGadget(),
		TestGadget()
	};
	QList<TestGadget> o2 = {
		TestGadget(),
		TestGadget(),
		TestGadget()
	};
	QList<TestGadget> o3 = {
		TestGadget(),
		TestGadget(),
		TestGadget()
	};
	QTest::newRow("QList<QList<TestGadget>>") << QVariant::fromValue<QList<QList<TestGadget>>>({o1, o2, o3})
											   << (int)QVariant::List;
}

void GadgetSerializerTest::testVariantConversions()
{
	QFETCH(QVariant, data);
	QFETCH(int, targetType);

	auto origType = data.userType();
	auto convData = data;
	QVERIFY(convData.convert(targetType));
	QVERIFY(convData.convert(origType));
	QCOMPARE(convData, data);
}

void GadgetSerializerTest::testSerialization_data()
{
	QTest::addColumn<ParentGadget>("gadget");
	QTest::addColumn<QJsonObject>("result");

	generateValidTestData();
}

void GadgetSerializerTest::testSerialization()
{
	QFETCH(ParentGadget, gadget);
	QFETCH(QJsonObject, result);

	QCOMPARE(serializer->serialize(gadget), result);
}

void GadgetSerializerTest::testInvalidSerialization()
{
	QVERIFY_EXCEPTION_THROWN(serializer->serialize(BrokenTestGadget()), SerializerException);
}

void GadgetSerializerTest::testDeserialization_data()
{
	QTest::addColumn<ParentGadget>("gadget");
	QTest::addColumn<QJsonObject>("data");

	generateValidTestData();
}

void GadgetSerializerTest::testDeserialization()
{
	QFETCH(QJsonObject, data);
	QFETCH(ParentGadget, gadget);

	QCOMPARE(serializer->deserialize<ParentGadget>(data), gadget);
}

void GadgetSerializerTest::testInvalidDeserialization()
{
	QJsonObject broken({
						  {"intProperty", 0},
						  {"boolProperty", false},
						  {"stringProperty", QString()},
						  {"doubleProperty", 0},
						  {"simpeList", QJsonArray()},
						  {"leveledList", QJsonArray()},
						  {"childGadget", QJsonObject({
							   {"intProperty", 0},
							   {"boolProperty", false},
							   {"stringProperty", QString()},
							   {"doubleProperty", 0},
							   {"simpeList", QJsonArray()},
							   {"leveledList", QJsonArray()}
						   })},
						  {"simpleChildren", QJsonArray()},
						  {"leveledChildren", QJsonArray()},
						  {"broken", QJsonValue::Null}
					  });

	QVERIFY_EXCEPTION_THROWN(serializer->deserialize<BrokenTestGadget>(broken), SerializerException);
}

void GadgetSerializerTest::generateValidTestData()
{
	QTest::newRow("default") << ParentGadget()
							 << QJsonObject({
												{"intProperty", 0},
												{"boolProperty", false},
												{"stringProperty", QString()},
												{"doubleProperty", 0},
												{"simpeList", QJsonArray()},
												{"leveledList", QJsonArray()},
												{"childGadget", QJsonObject({
													 {"intProperty", 0},
													 {"boolProperty", false},
													 {"stringProperty", QString()},
													 {"doubleProperty", 0},
													 {"simpeList", QJsonArray()},
													 {"leveledList", QJsonArray()}
												 })},
												{"simpleChildren", QJsonArray()},
												{"leveledChildren", QJsonArray()}
											});

	QTest::newRow("basic") << ParentGadget::createBasic(42, true, "baum", 4.2)
						   << QJsonObject({
											  {"intProperty", 42},
											  {"boolProperty", true},
											  {"stringProperty", QStringLiteral("baum")},
											  {"doubleProperty", 4.2},
											  {"simpeList", QJsonArray()},
											  {"leveledList", QJsonArray()},
											  {"childGadget", QJsonObject({
												   {"intProperty", 0},
												   {"boolProperty", false},
												   {"stringProperty", QString()},
												   {"doubleProperty", 0},
												   {"simpeList", QJsonArray()},
												   {"leveledList", QJsonArray()}
											   })},
											  {"simpleChildren", QJsonArray()},
											  {"leveledChildren", QJsonArray()}
										  });

	QTest::newRow("list") << ParentGadget::createList({3, 7, 13}, {})
						  << QJsonObject({
											 {"intProperty", 0},
											 {"boolProperty", false},
											 {"stringProperty", QString()},
											 {"doubleProperty", 0},
											 {"simpeList", QJsonArray({3, 7, 13})},
											 {"leveledList", QJsonArray()},
											 {"childGadget", QJsonObject({
												  {"intProperty", 0},
												  {"boolProperty", false},
												  {"stringProperty", QString()},
												  {"doubleProperty", 0},
												  {"simpeList", QJsonArray()},
												  {"leveledList", QJsonArray()}
											  })},
											 {"simpleChildren", QJsonArray()},
											 {"leveledChildren", QJsonArray()}
										 });

	{
		QList<int> l1 = {0, 1, 2};
		QList<int> l2 = {3, 4, 5};
		QList<int> l3 = {6, 7, 8};
		QJsonArray j1 = {0, 1, 2};
		QJsonArray j2 = {3, 4, 5};
		QJsonArray j3 = {6, 7, 8};
		QTest::newRow("list<list>") << ParentGadget::createList({3, 7, 13}, {l1, l2, l3})
									<< QJsonObject({
													   {"intProperty", 0},
													   {"boolProperty", false},
													   {"stringProperty", QString()},
													   {"doubleProperty", 0},
													   {"simpeList", QJsonArray({3, 7, 13})},
													   {"leveledList", QJsonArray({j1, j2, j3})},
													   {"childGadget", QJsonObject({
															{"intProperty", 0},
															{"boolProperty", false},
															{"stringProperty", QString()},
															{"doubleProperty", 0},
															{"simpeList", QJsonArray()},
															{"leveledList", QJsonArray()}
														})},
													   {"simpleChildren", QJsonArray()},
													   {"leveledChildren", QJsonArray()}
												   });
	}

	QTest::newRow("child") << ParentGadget::createChild(ParentGadget::createBasic(42, true, "baum", 4.2), {}, {})
						   << QJsonObject({
											  {"intProperty", 0},
											  {"boolProperty", false},
											  {"stringProperty", QString()},
											  {"doubleProperty", 0},
											  {"simpeList", QJsonArray()},
											  {"leveledList", QJsonArray()},
											  {"childGadget", QJsonObject({
												   {"intProperty", 42},
												   {"boolProperty", true},
												   {"stringProperty", QStringLiteral("baum")},
												   {"doubleProperty", 4.2},
												   {"simpeList", QJsonArray()},
												   {"leveledList", QJsonArray()},
											   })},
											  {"simpleChildren", QJsonArray()},
											  {"leveledChildren", QJsonArray()}
										  });

	{
		TestGadget c1 = ParentGadget::createBasic(1, false, "1", 1.1);
		TestGadget c2 = ParentGadget::createBasic(2, true, "2", 2.2);
		TestGadget c3 = ParentGadget::createBasic(3, false, "3", 3.3);
		QJsonObject j1({
						   {"intProperty", 1},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("1")},
						   {"doubleProperty", 1.1},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j2({
						   {"intProperty", 2},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("2")},
						   {"doubleProperty", 2.2},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j3({
						   {"intProperty", 3},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("3")},
						   {"doubleProperty", 3.3},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QTest::newRow("childlist") << ParentGadget::createChild(TestGadget(), {c1, c2, c3}, {})
								   << QJsonObject({
													  {"intProperty", 0},
													  {"boolProperty", false},
													  {"stringProperty", QString()},
													  {"doubleProperty", 0},
													  {"simpeList", QJsonArray()},
													  {"leveledList", QJsonArray()},
													  {"childGadget", QJsonObject({
														   {"intProperty", 0},
														   {"boolProperty", false},
														   {"stringProperty", QString()},
														   {"doubleProperty", 0},
														   {"simpeList", QJsonArray()},
														   {"leveledList", QJsonArray()}
													   })},
													  {"simpleChildren", QJsonArray({j1, j2, j3})},
													  {"leveledChildren", QJsonArray()}
												  });
	}

	{
		TestGadget c0 = ParentGadget::createBasic(0, false, "0", 0.0);
		TestGadget c1 = ParentGadget::createBasic(1, true, "1", 1.1);
		TestGadget c2 = ParentGadget::createBasic(2, false, "2", 2.2);
		TestGadget c3 = ParentGadget::createBasic(3, true, "3", 3.3);
		TestGadget c4 = ParentGadget::createBasic(4, false, "4", 4.4);
		TestGadget c5 = ParentGadget::createBasic(5, true, "5", 5.5);
		TestGadget c6 = ParentGadget::createBasic(6, false, "6", 6.6);
		TestGadget c7 = ParentGadget::createBasic(7, true, "7", 7.7);
		TestGadget c8 = ParentGadget::createBasic(8, false, "8", 8.8);
		QList<QList<TestGadget>> ccl = {
			{c0, c1, c2},
			{c3, c4, c5},
			{c6, c7, c8}
		};
		QJsonObject j0({
						   {"intProperty", 0},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("0")},
						   {"doubleProperty", 0.0},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j1({
						   {"intProperty", 1},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("1")},
						   {"doubleProperty", 1.1},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j2({
						   {"intProperty", 2},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("2")},
						   {"doubleProperty", 2.2},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j3({
						   {"intProperty", 3},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("3")},
						   {"doubleProperty", 3.3},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j4({
						   {"intProperty", 4},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("4")},
						   {"doubleProperty", 4.4},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j5({
						   {"intProperty", 5},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("5")},
						   {"doubleProperty", 5.5},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j6({
						   {"intProperty", 6},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("6")},
						   {"doubleProperty", 6.6},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j7({
						   {"intProperty", 7},
						   {"boolProperty", true},
						   {"stringProperty", QStringLiteral("7")},
						   {"doubleProperty", 7.7},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonObject j8({
						   {"intProperty", 8},
						   {"boolProperty", false},
						   {"stringProperty", QStringLiteral("8")},
						   {"doubleProperty", 8.8},
						   {"simpeList", QJsonArray()},
						   {"leveledList", QJsonArray()}
					   });
		QJsonArray jjl = {
			QJsonArray({j0, j1, j2}),
			QJsonArray({j3, j4, j5}),
			QJsonArray({j6, j7, j8})
		};
		QTest::newRow("list<childlist>") << ParentGadget::createChild(TestGadget(), {}, ccl)
										 << QJsonObject({
															{"intProperty", 0},
															{"boolProperty", false},
															{"stringProperty", QString()},
															{"doubleProperty", 0},
															{"simpeList", QJsonArray()},
															{"leveledList", QJsonArray()},
															{"childGadget", QJsonObject({
																 {"intProperty", 0},
																 {"boolProperty", false},
																 {"stringProperty", QString()},
																 {"doubleProperty", 0},
																 {"simpeList", QJsonArray()},
																 {"leveledList", QJsonArray()}
															 })},
															{"simpleChildren", QJsonArray()},
															{"leveledChildren", jjl}
														});
	}
}

QTEST_MAIN(GadgetSerializerTest)

#include "tst_gadgetserializer.moc"
