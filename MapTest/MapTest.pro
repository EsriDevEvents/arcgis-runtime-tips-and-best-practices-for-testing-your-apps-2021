QT += testlib
QT += gui
CONFIG += qt warn_on depend_includepath testcase

ARCGIS_RUNTIME_VERSION = 100.10
include($$PWD/arcgisruntime.pri)

TEMPLATE = app

SOURCES +=  tst_map.cpp
