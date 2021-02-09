#include <QtTest>

#include "Map.h"
#include "OpenStreetMapLayer.h"

class MapTest : public QObject
{
  Q_OBJECT

public:
  MapTest() = default;
  ~MapTest() = default;

private slots:
  void test_1_simple_property();
  void test_2_async_method();
  void test_3_async_method2();
  void test_4_advanced_techniques();
  void test_5_advanced_techniques2();
  void test_6_data_driven_test_data();
  void test_6_data_driven_test();
  void test_7_with_failure();
  void test_8_with_unknown_failure();

private:
  const QUrl m_webMapUrl {"https://www.arcgis.com/home/webmap/viewer.html?webmap=56bb3556bc5542cfa217348b09c80efb"};
};

using namespace Esri::ArcGISRuntime;

void MapTest::test_1_simple_property()
{
  Map* map = new Map(this);

  map->operationalLayers()->append(new OpenStreetMapLayer(this));

  QCOMPARE(map->operationalLayers()->size(), 1);
  QCOMPARE(map->operationalLayers()->at(0)->layerType(), LayerType::OpenStreetMapLayer);
}

void MapTest::test_2_async_method()
{
  Map* map = new Map(m_webMapUrl, this);

  map->load();

  QTRY_COMPARE(map->loadStatus(), LoadStatus::Loaded);
}

void MapTest::test_3_async_method2()
{
  Map* map = new Map(m_webMapUrl, this);

  LoadStatus loadStatusFromSignal = LoadStatus::Unknown;
  connect(map, &Map::loadStatusChanged, this, [&loadStatusFromSignal](LoadStatus ls)
  {
    loadStatusFromSignal = ls;
  });

  map->load();

  QTRY_COMPARE(loadStatusFromSignal, LoadStatus::Loaded);
}

void MapTest::test_4_advanced_techniques()
{
  // Use RAII to make sure objects are destructed at end of test
  // Method 1 - use value objects
  Map map {m_webMapUrl};

  map.load();

  QTRY_COMPARE(map.loadStatus(), LoadStatus::Loaded);
}

void MapTest::test_5_advanced_techniques2()
{
  // Use RAII to make sure objects are destructed at end of test
  // Method 2 - use local parent
  QObject localParent;
  Map* map = new Map(m_webMapUrl, &localParent);

  map->load();

  QTRY_COMPARE(map->loadStatus(), LoadStatus::Loaded);
}

void MapTest::test_6_data_driven_test_data()
{
  QTest::addColumn<QUrl>("url");

  const QUrl webMap {"https://www.arcgis.com/home/webmap/viewer.html?webmap=56bb3556bc5542cfa217348b09c80efb"};
  const QUrl item {"https://www.arcgis.com/home/item.html?id=56bb3556bc5542cfa217348b09c80efb"};
  const QUrl dataSharing {"https://www.arcgis.com/sharing/rest/content/items/56bb3556bc5542cfa217348b09c80efb/data"};

  QTest::newRow("web map url") << webMap;
  QTest::newRow("item url") << item;
  QTest::newRow("data sharing url") << dataSharing;
}

void MapTest::test_6_data_driven_test()
{
  QFETCH(QUrl, url);

  Map* map = new Map(url, this);

  map->load();

  QTRY_COMPARE(map->loadStatus(), LoadStatus::Loaded);
}

void MapTest::test_7_with_failure()
{
  Map* map = new Map(m_webMapUrl, this);

  map->load();

  QTRY_COMPARE(map->loadStatus(), LoadStatus::Loaded);

  QCOMPARE(map->operationalLayers()->size(), 16);
}

void MapTest::test_8_with_unknown_failure()
{
  Map* map = new Map(m_webMapUrl, this);

  map->load();

  QTRY_COMPARE(map->loadStatus(), LoadStatus::Loaded);

//  QEXPECT_FAIL("", "unknown failure, need to follow up with GIS analyst", Abort);
  QCOMPARE(map->operationalLayers()->size(), 16);
}

QTEST_GUILESS_MAIN(MapTest)

#include "tst_map.moc"
