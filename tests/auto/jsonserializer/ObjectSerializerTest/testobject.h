#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QDateTime>
#include <QUuid>
#include <QUrl>
#include <QFlags>
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedPointer>
#include <QPointer>
#include <QVersionNumber>
#include <QSize>
#include <QPoint>
#include <QLine>
#include <QRect>
#include <QLocale>
#include <QRegularExpression>

class ChildObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int data MEMBER data)
	Q_PROPERTY(ChildObject *child MEMBER child)

public:
	int data;
	ChildObject *child;

	explicit ChildObject(int data, QObject *parent = nullptr);
	Q_INVOKABLE ChildObject(QObject *parent);

	static QJsonObject createJson(const int &data = 0);

	static bool equals(const ChildObject *left, const ChildObject *right);

private:
	bool equals(const ChildObject *other) const;
};

class TestObject : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int intProperty MEMBER intProperty)
	Q_PROPERTY(bool boolProperty MEMBER boolProperty)
	Q_PROPERTY(QString stringProperty MEMBER stringProperty)
	Q_PROPERTY(double doubleProperty MEMBER doubleProperty)

	Q_PROPERTY(NormalEnum normalEnumProperty MEMBER normalEnumProperty)
	Q_PROPERTY(EnumFlags enumFlagsProperty READ getEnumFlagsProperty WRITE setEnumFlagsProperty)

	Q_PROPERTY(QDateTime datetime MEMBER datetime)
	Q_PROPERTY(QUuid uuid MEMBER uuid)
	Q_PROPERTY(QUrl url MEMBER url)
	Q_PROPERTY(QVersionNumber version MEMBER version)
	Q_PROPERTY(QByteArray bytearray MEMBER bytearray)

	Q_PROPERTY(QSize size MEMBER size)
	Q_PROPERTY(QPoint point MEMBER point)
	Q_PROPERTY(QLine line MEMBER line)
	Q_PROPERTY(QRect rect MEMBER rect)

	Q_PROPERTY(QLocale locale MEMBER locale)
	Q_PROPERTY(QRegularExpression regexp MEMBER regexp)

	Q_PROPERTY(QList<int> simpleList MEMBER simpleList)
	Q_PROPERTY(QList<QList<int>> leveledList MEMBER leveledList)

	Q_PROPERTY(QMap<QString, int> simpleMap MEMBER simpleMap);
	Q_PROPERTY(QMap<QString, QMap<QString, int>> leveledMap MEMBER leveledMap);

	Q_PROPERTY(QPair<int, QString> pair MEMBER pair);
	Q_PROPERTY(QPair<ChildObject*, QList<int>> extraPair MEMBER extraPair);
	Q_PROPERTY(QList<QPair<bool, bool>> listPair MEMBER listPair);

	Q_PROPERTY(ChildObject* childObject MEMBER childObject)
	Q_PROPERTY(QSharedPointer<ChildObject> sharedChildObject MEMBER sharedChildObject)
	Q_PROPERTY(QPointer<ChildObject> trackedChildObject MEMBER trackedChildObject)

	Q_PROPERTY(QList<ChildObject*> simpleChildren MEMBER simpleChildren)
	Q_PROPERTY(QList<QList<ChildObject*>> leveledChildren MEMBER leveledChildren)

	Q_PROPERTY(QMap<QString, ChildObject*> simpleRelatives MEMBER simpleRelatives);
	Q_PROPERTY(QMap<QString, QMap<QString, ChildObject*>> leveledRelatives MEMBER leveledRelatives);

	Q_PROPERTY(QJsonObject object MEMBER object)
	Q_PROPERTY(QJsonArray array MEMBER array)
	Q_PROPERTY(QJsonValue value MEMBER value)

	Q_PROPERTY(std::tuple<int, QString, QList<int>> stdTuple MEMBER stdTuple);
	Q_PROPERTY(std::pair<bool, int> stdPair MEMBER stdPair);

public:
	enum NormalEnum {
		Normal0 = 0,
		Normal1 = 1,
		Normal2 = 2
	};
	Q_ENUM(NormalEnum)
	enum EnumFlag {
		Flag1 = 0x02,
		Flag2 = 0x04,
		Flag3 = 0x08,

		FlagX = Flag1 | Flag2
	};
	Q_DECLARE_FLAGS(EnumFlags, EnumFlag)
	Q_FLAG(EnumFlags)

	Q_INVOKABLE TestObject(QObject *parent);

	static TestObject *createBasic(int intProperty, bool boolProperty, QString stringProperty, double doubleProperty, QObject *parent);
	static TestObject *createEnum(NormalEnum normalEnumProperty, EnumFlags enumFlagsProperty, QObject *parent);
	static TestObject *createExtra(QDateTime datetime, QUuid uuid, QUrl url, QVersionNumber version, QByteArray bytearray, QObject *parent);
	static TestObject *createGeom(QSize size, QPoint point, QLine line, QRect rect, QObject *parent);
	static TestObject *createSpecial(QLocale locale, QRegularExpression regexp, QObject *parent);
	static TestObject *createList(QList<int> simpleList, QList<QList<int>> leveledList, QObject *parent);
	static TestObject *createMap(QMap<QString, int> simpleMap, QMap<QString, QMap<QString, int>> leveledMap, QObject *parent);
	static TestObject *createPair(QPair<int, QString> pair, QPair<ChildObject*, QList<int>> extraPair, QList<QPair<bool, bool>> listPair, QObject *parent);
	static TestObject *createChild(ChildObject* childObject, QObject *parent, int memberFlag = 0);//0: ptr, 1: sp, 2: tp
	static TestObject *createChildren(QList<ChildObject*> simpleChildren, QList<QList<ChildObject*>> leveledChildren, QObject *parent);
	static TestObject *createRelatives(QMap<QString, ChildObject*> simpleRelatives, QMap<QString, QMap<QString, ChildObject*>> leveledRelatives, QObject *parent);
	static TestObject *createEmbedded(QJsonObject object, QJsonArray array, QJsonValue value, QObject *parent);
	static TestObject *createStdTuple(int v1, QString v2, QList<int> v3, QObject *parent);
	static TestObject *createStdPair(bool first, int second, QObject *parent);

	static QJsonObject createJson(const QJsonObject &delta = QJsonObject(), const QString &rmKey = {});

	static bool equals(const TestObject *left, const TestObject *right);

	int intProperty = 0;
	bool boolProperty = false;
	QString stringProperty;
	double doubleProperty = 0.0;

	NormalEnum normalEnumProperty = Normal0;
	EnumFlags enumFlagsProperty;

	QDateTime datetime;
	QUuid uuid;
	QUrl url;
	QVersionNumber version;
	QByteArray bytearray;

	QSize size;
	QPoint point;
	QLine line;
	QRect rect;

	QLocale locale = QLocale::c();
	QRegularExpression regexp;

	QList<int> simpleList;
	QList<QList<int>> leveledList;

	QMap<QString, int> simpleMap;
	QMap<QString, QMap<QString, int>> leveledMap;

	QPair<int, QString> pair;
	QPair<ChildObject*, QList<int>> extraPair;
	QList<QPair<bool, bool>> listPair;

	ChildObject* childObject = nullptr;
	QSharedPointer<ChildObject> sharedChildObject;
	QPointer<ChildObject> trackedChildObject;

	QList<ChildObject*> simpleChildren;
	QList<QList<ChildObject*>> leveledChildren;

	QMap<QString, ChildObject*> simpleRelatives;
	QMap<QString, QMap<QString, ChildObject*>> leveledRelatives;

	QJsonObject object;
	QJsonArray array;
	QJsonValue value = QJsonValue::Null;

	std::tuple<int, QString, QList<int>> stdTuple {0, {}, {}};
	std::pair<bool, int> stdPair {false, 0};

	EnumFlags getEnumFlagsProperty() const;
	void setEnumFlagsProperty(const EnumFlags &value);

private:
	bool equals(const TestObject *other) const;
};

Q_DECLARE_METATYPE(TestObject*)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestObject::EnumFlags)

#endif // TESTOBJECT_H
