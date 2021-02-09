CONFIG += warn_on qmltestcase

TEMPLATE = app

ARCGIS_RUNTIME_VERSION = 100.10
include($$PWD/arcgisruntime.pri)

DISTFILES += tst_maptestqmlgui.qml

SOURCES += main.cpp

HEADERS += Setup.h
