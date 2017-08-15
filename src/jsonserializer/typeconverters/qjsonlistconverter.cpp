#include "qjsonlistconverter_p.h"
#include "qjsonserializerexception.h"

const QRegularExpression QJsonListConverter::listTypeRegex(QStringLiteral(R"__(^QList<\s*(.*)\s*>$)__"));

bool QJsonListConverter::canConvert(int metaTypeId) const
{
	return metaTypeId == QMetaType::QVariantList ||
			metaTypeId == QMetaType::QStringList ||
			metaTypeId == QMetaType::QByteArrayList ||
			listTypeRegex.match(QString::fromUtf8(QMetaType::typeName(metaTypeId))).hasMatch();
}

QList<QJsonValue::Type> QJsonListConverter::jsonTypes() const
{
	return {QJsonValue::Array};
}

QJsonValue QJsonListConverter::serialize(int propertyType, const QVariant &value, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto metaType = getSubtype(propertyType);

	auto cValue = value;
	if(!cValue.convert(QVariant::List)) {
		throw QJsonSerializationException(QByteArray("Failed to convert type ") +
										  QMetaType::typeName(propertyType) +
										  QByteArray(" to a variant list"));
	}

	QJsonArray array;
	foreach(auto element, cValue.toList())
		array.append(helper->serializeSubtype(metaType, element));
	return array;
}

QVariant QJsonListConverter::deserialize(int propertyType, const QJsonValue &value, QObject *parent, const QJsonTypeConverter::SerializationHelper *helper) const
{
	auto metaType = getSubtype(propertyType);

	//generate the list
	QVariantList list;
	foreach(auto element, value.toArray())
		list.append(helper->deserializeSubtype(metaType, element, parent));
	return list;
}

int QJsonListConverter::getSubtype(int listType)
{
	int metaType = QMetaType::UnknownType;
	if(listType == QMetaType::QStringList)
		metaType = QMetaType::QString;
	else if(listType == QMetaType::QByteArrayList)
		metaType = QMetaType::QByteArray;
	else {
		auto match = listTypeRegex.match(QString::fromUtf8(QMetaType::typeName(listType)));
		if(match.hasMatch())
			metaType = QMetaType::type(match.captured(1).toUtf8().trimmed());
	}

	return metaType;
}
