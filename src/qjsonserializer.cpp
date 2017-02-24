#include "qjsonserializer.h"

#include <QRegularExpression>
#include <QCoreApplication>
#include <QDateTime>
#include <QUuid>
#include <QUrl>

static const QRegularExpression listTypeRegex(QStringLiteral(R"__(^QList<\s*(.*)\s*>$)__"));

static void qJsonSerializerStartup();
Q_COREAPP_STARTUP_FUNCTION(qJsonSerializerStartup)

QJsonSerializer::QJsonSerializer(QObject *parent) :
	QObject(parent),
	_allowNull(false),
	_keepObjectName(false)
{}

QJsonSerializer::~QJsonSerializer() {}

bool QJsonSerializer::allowDefaultNull() const
{
	return _allowNull;
}

bool QJsonSerializer::keepObjectName() const
{
	return _keepObjectName;
}

void QJsonSerializer::setAllowDefaultNull(bool allowDefaultNull)
{
	_allowNull = allowDefaultNull;
}

void QJsonSerializer::setKeepObjectName(bool keepObjectName)
{
	_keepObjectName = keepObjectName;
}

QJsonValue QJsonSerializer::serializeVariant(int propertyType, const QVariant &value) const
{
	auto convertValue = value;
	if((propertyType == QVariant::List) ||
	   (convertValue.canConvert(QVariant::List) && convertValue.convert(QVariant::List))) {
		return serializeList(propertyType, value.toList());
	} else {
		convertValue = value;
		auto flags = QMetaType::typeFlags(propertyType);

		if(flags.testFlag(QMetaType::IsGadget)) {
			auto metaObject = QMetaType::metaObjectForType(propertyType);
			return serializeGadget(value.constData(), metaObject);
		} else if(flags.testFlag(QMetaType::PointerToQObject) ||
				  (convertValue.canConvert(QMetaType::QObjectStar) && convertValue.convert(QMetaType::QObjectStar))) {
			auto object = convertValue.value<QObject*>();
			if(object)
				return serializeObject(object);
			else
				return QJsonValue::Null;
		} else
			return serializeValue(propertyType, value);
	}
}

QJsonObject QJsonSerializer::serializeObject(const QObject *object) const
{
	auto meta = object->metaObject();

	QJsonObject jsonObject;
	//go through all properties and try to serialize them
	auto i = QObject::staticMetaObject.indexOfProperty("objectName");
	if(!_keepObjectName)
	   i++;
	for(; i < meta->propertyCount(); i++) {
		auto property = meta->property(i);
		if(property.isStored())
			jsonObject[property.name()]= serializeVariant(property.userType(), property.read(object));
	}

	return jsonObject;
}

QJsonObject QJsonSerializer::serializeGadget(const void *gadget, const QMetaObject *metaObject) const
{
	QJsonObject jsonObject;
	//go through all properties and try to serialize them
	for(auto i = 0; i < metaObject->propertyCount(); i++) {
		auto property = metaObject->property(i);
		if(property.isStored())
			jsonObject[property.name()]= serializeVariant(property.userType(), property.readOnGadget(gadget));
	}

	return jsonObject;
}

QJsonArray QJsonSerializer::serializeList(int listType, const QVariantList &value) const
{
	auto match = listTypeRegex.match(QMetaType::typeName(listType));
	int metaType = QMetaType::UnknownType;
	if(match.hasMatch())
		metaType = QMetaType::type(match.captured(1).toLatin1());

	QJsonArray array;
	foreach(auto element, value)
		array.append(serializeVariant(metaType, element));
	return array;
}

QJsonValue QJsonSerializer::serializeValue(int propertyType, QVariant value) const
{
	if(!value.isValid())
		return QJsonValue::Null;
	else {
		auto json = QJsonValue::fromVariant(value);
		if(json.isNull()) {
			if(propertyType == QMetaType::QDate ||
			   propertyType == QMetaType::QTime ||
			   propertyType == QMetaType::QDateTime ||
			   value.userType() == QMetaType::QDate ||
			   value.userType() == QMetaType::QTime ||
			   value.userType() == QMetaType::QDateTime)
				return QJsonValue::String;//special case date: invalid date -> empty string -> interpret as fail
			else
				throw QJsonSerializationException(QStringLiteral("Failed to convert type %1 to a JSON representation").arg(value.typeName()));
		}
		else
			return json;
	}
}

QVariant QJsonSerializer::deserializeVariant(int propertyType, const QJsonValue &value, QObject *parent) const
{
	QVariant variant;
	if(value.isArray()) {
		variant = deserializeList(propertyType, value.toArray(), parent);
	} else if(value.isObject() || value.isNull()) {
		auto flags = QMetaType::typeFlags(propertyType);

		if(flags.testFlag(QMetaType::IsGadget)) {
			QVariant gadget(propertyType, nullptr);
			deserializeGadget(value.toObject(), propertyType, gadget.data());
			variant = gadget;
		} else if(flags.testFlag(QMetaType::PointerToQObject)) {
			if(value.isNull())
				variant = QVariant::fromValue<QObject*>(nullptr);
			else {
				auto object = deserializeObject(value.toObject(), QMetaType::metaObjectForType(propertyType), parent);
				variant = QVariant::fromValue(object);
			}
		} else
			variant = deserializeValue(propertyType, value);
	} else
		variant = deserializeValue(propertyType, value);

	if(propertyType != QMetaType::UnknownType) {
		auto vType = variant.typeName();
		if(variant.canConvert(propertyType) && variant.convert(propertyType))
			return variant;
		else if(_allowNull && value.isNull())
			return QVariant();
		else {
			throw QJsonDeserializationException(QStringLiteral("Failed to convert deserialized variant of type %1 to property type %2")
										   .arg(vType ? vType : "<unknown>")
										   .arg(QMetaType::typeName(propertyType)));
		}
	} else
		return variant;
}

QObject *QJsonSerializer::deserializeObject(QJsonObject jsonObject, const QMetaObject *metaObject, QObject *parent) const
{
	//try to construct the object
	auto object = metaObject->newInstance(Q_ARG(QObject*, parent));
	if(!object)
		throw QJsonDeserializationException(QStringLiteral("Failed to construct object of type %1 (Does the constructor \"Q_INVOKABLE class(QObject*);\" exist?)").arg(metaObject->className()));

	//now deserialize all json properties
	for(auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++) {
		auto propIndex = metaObject->indexOfProperty(qUtf8Printable(it.key()));
		auto type = propIndex != -1 ?
						metaObject->property(propIndex).userType() :
						QMetaType::UnknownType;
		object->setProperty(qUtf8Printable(it.key()), deserializeVariant(type, it.value(), object));
	}

	return object;
}

void QJsonSerializer::deserializeGadget(QJsonObject jsonObject, int typeId, void *gadgetPtr) const
{
	auto metaObject = QMetaType::metaObjectForType(typeId);
	if(!QMetaType::construct(typeId, gadgetPtr, nullptr))
		throw QJsonDeserializationException(QStringLiteral("Failed to construct gadget of type %1").arg(QMetaType::typeName(typeId)));

	//now deserialize all json properties
	for(auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++) {
		auto propIndex = metaObject->indexOfProperty(qUtf8Printable(it.key()));
		if(propIndex != -1) {
			auto property = metaObject->property(propIndex);
			property.writeOnGadget(gadgetPtr, deserializeVariant(property.userType(), it.value(), nullptr));
		}
	}
}

QVariantList QJsonSerializer::deserializeList(int listType, const QJsonArray &array, QObject *parent) const
{
	int metaType = QMetaType::UnknownType;
	if(listType != QMetaType::UnknownType) {
		auto match = listTypeRegex.match(QMetaType::typeName(listType));
		if(match.hasMatch())
			metaType = QMetaType::type(match.captured(1).toLatin1());
	}

	//generate the list
	QVariantList list;
	foreach(auto element, array)
		list.append(deserializeVariant(metaType, element, parent));
	return list;
}

QVariant QJsonSerializer::deserializeValue(int propertyType, QJsonValue value) const
{
	Q_UNUSED(propertyType);
	return value.toVariant();//all json can be converted to qvariant
}

// ------------- Startup function implementation -------------

static void qJsonSerializerStartup()
{
	QJsonSerializer::registerListConverters<bool>();
	QJsonSerializer::registerListConverters<int>();
	QJsonSerializer::registerListConverters<unsigned int>();
	QJsonSerializer::registerListConverters<double>();
	QJsonSerializer::registerListConverters<QChar>();
	QJsonSerializer::registerListConverters<QString>();
	QJsonSerializer::registerListConverters<long long>();
	QJsonSerializer::registerListConverters<short>();
	QJsonSerializer::registerListConverters<char>();
	QJsonSerializer::registerListConverters<unsigned long>();
	QJsonSerializer::registerListConverters<unsigned long long>();
	QJsonSerializer::registerListConverters<unsigned short>();
	QJsonSerializer::registerListConverters<signed char>();
	QJsonSerializer::registerListConverters<unsigned char>();
	QJsonSerializer::registerListConverters<float>();
	QJsonSerializer::registerListConverters<QDate>();
	QJsonSerializer::registerListConverters<QTime>();
	QJsonSerializer::registerListConverters<QUrl>();
	QJsonSerializer::registerListConverters<QDateTime>();
	QJsonSerializer::registerListConverters<QUuid>();
	QJsonSerializer::registerListConverters<QObject*>();
}
