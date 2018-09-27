#include "testgadget.h"
#include <QJsonArray>
#include <QMetaObject>
#include <QMetaProperty>

bool TestGadget::operator==(const TestGadget &other) const
{
	return intProperty == other.intProperty &&
			boolProperty == other.boolProperty &&
			stringProperty == other.stringProperty &&
			qFuzzyCompare(doubleProperty, other.doubleProperty) &&
			normalEnumProperty == other.normalEnumProperty &&
			enumFlagsProperty == other.enumFlagsProperty &&
			datetime == other.datetime &&
			uuid == other.uuid &&
			url == other.url &&
			version == other.version &&
			bytearray == other.bytearray &&
			size == other.size &&
			point == other.point &&
			line == other.line &&
			rect == other.rect &&
			locale.bcp47Name() == other.locale.bcp47Name() &&
			regexp == other.regexp &&
			simpleList == other.simpleList &&
			leveledList == other.leveledList &&
			simpleMap == other.simpleMap &&
			leveledMap == other.leveledMap &&
			pair == other.pair &&
			extraPair == other.extraPair &&
			listPair == other.listPair &&
			childGadget == other.childGadget &&
			simpleChildren == other.simpleChildren &&
			leveledChildren == other.leveledChildren &&
			simpleRelatives == other.simpleRelatives &&
			leveledRelatives == other.leveledRelatives &&
			object == other.object &&
			array == other.array &&
			value == other.value &&
			ChildGadget::equals(gadgetPtr, other.gadgetPtr) &&
			stdTuple == other.stdTuple;
}

bool TestGadget::operator!=(const TestGadget &other) const
{
	return intProperty != other.intProperty ||
			boolProperty != other.boolProperty ||
			stringProperty != other.stringProperty ||
			!qFuzzyCompare(doubleProperty, other.doubleProperty) ||
			normalEnumProperty != other.normalEnumProperty ||
			enumFlagsProperty != other.enumFlagsProperty ||
			datetime != other.datetime ||
			uuid != other.uuid ||
			url != other.url ||
			version != other.version ||
			bytearray != other.bytearray ||
			size != other.size ||
			point != other.point ||
			line != other.line ||
			rect != other.rect ||
			locale.bcp47Name() != other.locale.bcp47Name() ||
			regexp != other.regexp ||
			simpleList != other.simpleList ||
			leveledList != other.leveledList ||
			simpleMap != other.simpleMap ||
			leveledMap != other.leveledMap ||
			pair != other.pair ||
			extraPair != other.extraPair ||
			listPair != other.listPair ||
			childGadget != other.childGadget ||
			simpleChildren != other.simpleChildren ||
			leveledChildren != other.leveledChildren ||
			simpleRelatives != other.simpleRelatives ||
			leveledRelatives != other.leveledRelatives ||
			object != other.object ||
			array != other.array ||
			value != other.value ||
			!ChildGadget::equals(gadgetPtr, other.gadgetPtr) ||
			stdTuple != other.stdTuple;
}

bool TestGadget::operator<(const TestGadget &) const
{
	return false;
}

TestGadget::EnumFlags TestGadget::getEnumFlagsProperty() const
{
	return enumFlagsProperty;
}

void TestGadget::setEnumFlagsProperty(const EnumFlags &value)
{
	enumFlagsProperty = value;
}

TestGadget TestGadget::createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty)
{
	TestGadget t;
	t.intProperty = intProperty;
	t.boolProperty = boolProperty;
	t.stringProperty = std::move(stringProperty);
	t.doubleProperty = doubleProperty;
	return t;
}

TestGadget TestGadget::createEnum(TestGadget::NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty)
{
	TestGadget t;
	t.normalEnumProperty = normalEnumProperty;
	t.enumFlagsProperty = enumFlagsProperty;
	return t;
}

TestGadget TestGadget::createExtra(QDateTime datetime, QUuid uuid, QUrl url, QVersionNumber version, QByteArray bytearray)
{
	TestGadget t;
	t.datetime = std::move(datetime);
	t.uuid = uuid;
	t.url = std::move(url);
	t.version = std::move(version);
	t.bytearray = std::move(bytearray);
	return t;
}

TestGadget TestGadget::createGeom(QSize size, QPoint point, QLine line, QRect rect)
{
	TestGadget t;
	t.size = size;
	t.point = point;
	t.line = line;
	t.rect = rect;
	return t;
}

TestGadget TestGadget::createSpecial(QLocale locale, QRegularExpression regexp)
{
	TestGadget t;
	t.locale = std::move(locale);
	t.regexp = std::move(regexp);
	return t;
}

TestGadget TestGadget::createList(QList<int> simpleList, QList<QList<int> > leveledList)
{
	TestGadget t;
	t.simpleList = std::move(simpleList);
	t.leveledList = std::move(leveledList);
	return t;
}

TestGadget TestGadget::createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int> > leveledMap)
{
	TestGadget t;
	t.simpleMap = std::move(simpleMap);
	t.leveledMap = std::move(leveledMap);
	return t;
}

TestGadget TestGadget::createPair(QPair<int, QString> pair, QPair<ChildGadget, QList<int> > extraPair, QList<QPair<bool, bool> > listPair)
{
	TestGadget t;
	t.pair = std::move(pair);
	t.extraPair = std::move(extraPair);
	t.listPair = std::move(listPair);
	return t;
}

TestGadget TestGadget::createChild(ChildGadget childGadget)
{
	TestGadget t;
	t.childGadget = childGadget;
	return t;
}

TestGadget TestGadget::createChildren(QList<ChildGadget> simpleChildren, QList<QList<ChildGadget> > leveledChildren)
{
	TestGadget t;
	t.simpleChildren = std::move(simpleChildren);
	t.leveledChildren = std::move(leveledChildren);
	return t;
}

TestGadget TestGadget::createRelatives(QMap<QString, ChildGadget> simpleRelatives, QMap<QString, QMap<QString, ChildGadget> > leveledRelatives)
{
	TestGadget t;
	t.simpleRelatives = std::move(simpleRelatives);
	t.leveledRelatives = std::move(leveledRelatives);
	return t;
}

TestGadget TestGadget::createEmbedded(QJsonObject object, QJsonArray array, QJsonValue value)
{
	TestGadget t;
	t.object = std::move(object);
	t.array = std::move(array);
	t.value = std::move(value);
	return t;
}

TestGadget TestGadget::createGadgetPtr(ChildGadget *gadgetPtr)
{
	TestGadget t;
	t.gadgetPtr = gadgetPtr;
	return t;
}

TestGadget TestGadget::createStdTuple(int v1, QString v2, QList<int> v3)
{
	TestGadget t;
	t.stdTuple = std::make_tuple(v1, std::move(v2), std::move(v3));
	return t;
}

QJsonObject TestGadget::createJson(const QJsonObject &delta, const QString &rmKey)
{
	auto base = QJsonObject({
								{"intProperty", 0},
								{"boolProperty", false},
								{"stringProperty", QString()},
								{"doubleProperty", 0},
								{"normalEnumProperty", TestGadget::Normal0},
								{"enumFlagsProperty", 0},
								{"datetime", QString()},
								{"uuid", QStringLiteral("00000000-0000-0000-0000-000000000000")},
								{"url", QString()},
								{"version", QString()},
								{"bytearray", QString()},
								{"size", QJsonObject({
									 {"width", -1},
									 {"height", -1}
								 })},
								{"point", QJsonObject({
									 {"x", 0},
									 {"y", 0}
								 })},
								{"line", QJsonObject({
									 {"p1", QJsonObject({
										  {"x", 0},
										  {"y", 0}
									  })},
									 {"p2", QJsonObject({
										  {"x", 0},
										  {"y", 0}
									  })}
								 })},
								{"rect", QJsonObject({
									 {"topLeft", QJsonObject({
										  {"x", 0},
										  {"y", 0}
									  })},
									 {"bottomRight", QJsonObject({
										  {"x", -1},
										  {"y", -1}
									  })}
								 })},
								{"locale", QLocale::c().bcp47Name()},
								{"regexp", QJsonObject({
									 {"pattern", QString()},
									 {"options", 0}
								 })},
								{"simpleList", QJsonArray()},
								{"leveledList", QJsonArray()},
								{"simpleMap", QJsonObject()},
								{"leveledMap", QJsonObject()},
								{"pair", QJsonArray({0, QString()})},
								{"extraPair", QJsonArray({ChildGadget::createJson(), QJsonArray()})},
								{"listPair", QJsonArray()},
								{"simpleList", QJsonArray()},
								{"simpleList", QJsonArray()},
								{"childGadget", ChildGadget::createJson()},
								{"simpleChildren", QJsonArray()},
								{"leveledChildren", QJsonArray()},
								{"simpleRelatives", QJsonObject()},
								{"leveledRelatives", QJsonObject()},
								{"object", QJsonObject()},
								{"array", QJsonArray()},
								{"value", QJsonValue::Null},
								{"gadgetPtr", QJsonValue::Null},
								{"stdTuple", QJsonArray{0, QString{}, QJsonArray{}}}
							});
	for(auto it = delta.constBegin(); it != delta.constEnd(); ++it)
		base[it.key()] = it.value();
	base.remove(rmKey);
	return base;
}

ChildGadget::ChildGadget(int data) :
	data(data)
{}

bool ChildGadget::operator==(const ChildGadget &other) const
{
	return data == other.data;
}

bool ChildGadget::operator!=(const ChildGadget &other) const
{
	return data != other.data;
}

bool ChildGadget::operator<(const ChildGadget &other) const
{
	return data < other.data;
}

QJsonObject ChildGadget::createJson(const int &data)
{
	return QJsonObject({
						   {"data", data},
					   });
}

bool ChildGadget::equals(ChildGadget *lhs, ChildGadget *rhs)
{
	if(lhs == rhs)
		return true;
	if(lhs && rhs)
		return *lhs == *rhs;
	else
		return false;
}
