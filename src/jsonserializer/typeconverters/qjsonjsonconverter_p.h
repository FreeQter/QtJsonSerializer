#ifndef QJSONJSONCONVERTER_P_H
#define QJSONJSONCONVERTER_P_H

#include "qtjsonserializer_global.h"
#include "qjsontypeconverter.h"

class QJsonJsonConverter : public QSimpleJsonTypeConverter //TODO one converter per type...
{
public:
	bool canConvert(int metaTypeId) const override;
	QList<QJsonValue::Type> jsonTypes() const override;
	QJsonValue serialize(int propertyType, const QVariant &value) const override;
	QVariant deserialize(int propertyType, const QJsonValue &value, QObject *parent) const override;
};

#endif // QJSONJSONCONVERTER_P_H
