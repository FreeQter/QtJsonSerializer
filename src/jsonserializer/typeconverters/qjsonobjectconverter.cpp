#include "qjsonobjectconverter_p.h"
#include "qjsonserializerexception.h"
#include "qjsonserializer_p.h"

#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>

bool QJsonObjectConverter::canConvert(int metaTypeId) const
{
	auto flags = QMetaType::typeFlags(metaTypeId);
	return flags.testFlag(QMetaType::PointerToQObject) ||
			flags.testFlag(QMetaType::SharedPointerToQObject) ||//weak ptr cannot be constructed
			flags.testFlag(QMetaType::TrackingPointerToQObject);//TODO test all types
}

QList<QJsonValue::Type> QJsonObjectConverter::jsonTypes() const
{
	return {
		QJsonValue::Object,
		QJsonValue::Null
	};
}

QJsonValue QJsonObjectConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	QObject *object = nullptr;
	auto flags = QMetaType::typeFlags(propertyType);
	if(flags.testFlag(QMetaType::PointerToQObject))
		object = extract<QObject*>(value);
	else if(flags.testFlag(QMetaType::SharedPointerToQObject))
		object = extract<QSharedPointer<QObject>>(value).data();
	else if(flags.testFlag(QMetaType::TrackingPointerToQObject))
		object = extract<QPointer<QObject>>(value).data();
	else
		Q_UNREACHABLE();

	if(object) {
		auto meta = object->metaObject();
		auto d = QJsonSerializerPrivate::fromHelper(helper);

		QJsonObject jsonObject;
		//go through all properties and try to serialize them
		auto i = QObject::staticMetaObject.indexOfProperty("objectName");
		if(!d->keepObjectName)
		   i++;
		for(; i < meta->propertyCount(); i++) {
			auto property = meta->property(i);
			if(property.isStored())
				jsonObject[QString::fromUtf8(property.name())] = helper->serializeSubtype(property.userType(), property.read(object));
		}

		return jsonObject;
	}
	else
		return QJsonValue();
}

QVariant QJsonObjectConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	if(value.isNull())
		return QVariant::fromValue<QObject*>(nullptr);
	else {
		auto d = QJsonSerializerPrivate::fromHelper(helper);
		auto metaObject = QMetaType::metaObjectForType(propertyType);
		if(!metaObject)
			throw QJsonDeserializationException(QByteArray("Unable to get metaobject for type") + QMetaType::typeName(propertyType));

		//try to construct the object
		auto object = metaObject->newInstance(Q_ARG(QObject*, parent));
		if(!object) {
			throw QJsonDeserializationException(QByteArray("Failed to construct object of type ") +
												metaObject->className() +
												QByteArray(" (Does the constructor \"Q_INVOKABLE class(QObject*);\" exist?)"));
		}

		//collect required properties, if set
		QSet<QByteArray> reqProps;
		if(d->validationFlags.testFlag(QJsonSerializer::AllProperties)) {
			auto i = QObject::staticMetaObject.indexOfProperty("objectName");
			if(!d->keepObjectName)
			   i++;
			for(; i < metaObject->propertyCount(); i++) {
				auto property = metaObject->property(i);
				if(property.isStored())
					reqProps.insert(property.name());
			}
		}

		//now deserialize all json properties
		auto jsonObject = value.toObject();
		for(auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++) {
			auto propIndex = metaObject->indexOfProperty(qUtf8Printable(it.key()));
			QVariant value;
			if(propIndex != -1) {
				auto property = metaObject->property(propIndex);
				value = helper->deserializeSubtype(property.userType(), it.value(), object);
				reqProps.remove(property.name());
			} else if(d->validationFlags.testFlag(QJsonSerializer::NoExtraProperties)) {
				throw QJsonDeserializationException("Found extra property " +
													it.key().toUtf8() +
													" but extra properties are not allowed");
			} else
				value = helper->deserializeSubtype(QMetaType::UnknownType, it.value(), object);
			object->setProperty(qUtf8Printable(it.key()), value);
		}

		//make shure all required properties have been read
		if(d->validationFlags.testFlag(QJsonSerializer::AllProperties) && !reqProps.isEmpty()) {
			throw QJsonDeserializationException(QByteArray("Not all properties for ") +
												metaObject->className() +
												QByteArray(" are present in the json object Missing properties: ") +
												reqProps.toList().join(", "));
		}

		auto flags = QMetaType::typeFlags(propertyType);
		if(flags.testFlag(QMetaType::PointerToQObject))
			return QVariant::fromValue(object);
		else if(flags.testFlag(QMetaType::SharedPointerToQObject))
			return QVariant::fromValue(QSharedPointer<QObject>(object));
		else if(flags.testFlag(QMetaType::TrackingPointerToQObject))
			return QVariant::fromValue<QPointer<QObject>>(object);
		else {
			Q_UNREACHABLE();
			return QVariant();
		}
	}
}

template<typename T>
T QJsonObjectConverter::extract(QVariant variant)
{
	auto id = qMetaTypeId<T>();
	if(variant.canConvert(id) && variant.convert(id))
		return variant.value<T>();
	else
		throw QJsonSerializationException(QByteArray("unable to get QObject pointer from type ") + QMetaType::typeName(id));
}
