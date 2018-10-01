TEMPLATE = subdirs

SUBDIRS += ObjectSerializerTest \
	GadgetSerializerTest \
	TypeConverterTestLib \
	BytearrayConverterTest \
	GadgetConverterTest \
	GeomConverterTest \
	JsonConverterTest \
	ListConverterTest \
	LocaleConverterTest \
	MapConverterTest

never: SUBDIRS += TypeConverterTest

BytearrayConverterTest.depends += TypeConverterTestLib
GadgetConverterTest.depends += TypeConverterTestLib
GeomConverterTest.depends += TypeConverterTestLib
JsonConverterTest.depends += TypeConverterTestLib
ListConverterTest.depends += TypeConverterTestLib
LocaleConverterTest.depends += TypeConverterTestLib
MapConverterTest.depends += TypeConverterTestLib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
