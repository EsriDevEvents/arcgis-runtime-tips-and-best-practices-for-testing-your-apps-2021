QT += testlib widgets
QT += gui
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES +=  tst_maptestgui.cpp

ARCGIS_RUNTIME_VERSION = 100.10
include($$PWD/arcgisruntime.pri)
