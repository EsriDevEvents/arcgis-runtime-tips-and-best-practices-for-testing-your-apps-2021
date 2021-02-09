#include <QtTest>
#include <QCoreApplication>
#include <QMainWindow>
#include <QDebug>
#include "MapGraphicsView.h"
#include "Map.h"
#include "Basemap.h"
#include "Graphic.h"
#include "SimpleMarkerSymbol.h"

using namespace Esri::ArcGISRuntime;

class MapTestGui : public QMainWindow
{
  Q_OBJECT

public:
  MapTestGui() = default;
  ~MapTestGui() = default;

  void setup()
  {
    m_graphicsView = new MapGraphicsView(this);
    m_graphicsView->setMinimumWidth(600);
    m_graphicsView->setMinimumHeight(800);
    setCentralWidget(m_graphicsView);
  }

  MapGraphicsView* m_graphicsView = nullptr;

private slots:
  void init();
  void test_1_wraparound();
  void test_2_no_wraparound();
  void test_3_identify_graphic();
};

void MapTestGui::init()
{
  setup();
}

void MapTestGui::test_1_wraparound()
{
  m_graphicsView->setWrapAroundMode(WrapAroundMode::EnabledWhenSupported);

  Map* map = new Map(Basemap::streets(this), this);
  m_graphicsView->setMap(map);

  // wait for the map to load
  QTRY_COMPARE(map->loadStatus(), LoadStatus::Loaded);

  // set a viewpoint at the edge of the wraparound boundary
  m_graphicsView->setViewpoint(Viewpoint(Envelope(-20200191.801510714,
                                                  4284.2465115027735,
                                                  -19459110.135313917,
                                                  983824.64175089251)),
                               2.5);

  // now wait for the view's draw status to be complete
  QTRY_COMPARE(m_graphicsView->drawStatus(), DrawStatus::Completed);

  // advanced testing - check pixel data on left-most pixels to see if
  // basemap with blue ocean was rendered
  //m_graphicsView->exportImage();

  // this is not needed for the test, it is only to allow a visual inspection
  // for demonstration purposes
  QTest::qWait(3000);
}

void MapTestGui::test_2_no_wraparound()
{
  m_graphicsView->setWrapAroundMode(WrapAroundMode::Disabled);

  Map* map = new Map(Basemap::streets(this), this);
  m_graphicsView->setMap(map);

  // wait for the map to load
  QTRY_COMPARE(map->loadStatus(), LoadStatus::Loaded);

  // set a viewpoint at the edge of the wraparound boundary
  m_graphicsView->setViewpoint(Viewpoint(Envelope(-20200191.801510714,
                                                  4284.2465115027735,
                                                  -19459110.135313917,
                                                  983824.64175089251)),
                               2.5);

  // advanced testing - check pixel data on left-most pixels to see if
  // gray map background is displayed since wraparound was disabled
  //m_graphicsView->exportImage();

  // now wait for the view's draw status to be complete
  QTRY_COMPARE(m_graphicsView->drawStatus(), DrawStatus::Completed);

  // this is not needed for the test, it is only to allow a visual inspection
  // for demonstration purposes
  QTest::qWait(3000);
}

void MapTestGui::test_3_identify_graphic()
{
  Map* map = new Map(Basemap::nationalGeographic(this), this);
  m_graphicsView->setMap(map);

  // wait for the map to load
  QTRY_COMPARE(map->loadStatus(), LoadStatus::Loaded);

  // get the center point of the window
  const int screenCenterX = m_graphicsView->widthInPixels() / 2;
  const int screenCenterY = m_graphicsView->heightInPixels() / 2;

  // convert the screen coordinates to map coordinates
  const Point mapCoordinatesScreenCenter = m_graphicsView->screenToLocation(screenCenterX, screenCenterY);

  // add a graphics overlay and a graphic
  Graphic* graphic = new Graphic(mapCoordinatesScreenCenter,
                                 new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Square,
                                                        Qt::red,
                                                        65,
                                                        this),
                                 this);

  GraphicsOverlay* overlay = new GraphicsOverlay(this);

  m_graphicsView->graphicsOverlays()->append(overlay);

  overlay->graphics()->append(graphic);

  // now wait for the view's draw status to be complete
  QTRY_COMPARE(m_graphicsView->drawStatus(), DrawStatus::Completed);

  // connect to the signal for identifying graphics
  connect(m_graphicsView, &MapGraphicsView::identifyGraphicsOverlayCompleted, this,
          [](QUuid, IdentifyGraphicsOverlayResult* identifyResult)
  {
    if (identifyResult && !identifyResult->graphics().isEmpty())
      identifyResult->graphics().at(0)->setSelected(true);
  });

  // let's pause here for 3 seconds so the graphic can be seen in it's unselected state
  QTest::qWait(3000);

  // now identify the graphic in the center position on the screen
  m_graphicsView->identifyGraphicsOverlay(overlay, screenCenterX, screenCenterY, 5, false);

  // after we process the identify, we will select the graphic
  QTRY_COMPARE(graphic->isSelected(), true);

  // this is not needed for the test, it is only to allow a visual inspection
  // for demonstration purposes
  QTest::qWait(3000);
}

QTEST_MAIN(MapTestGui)

#include "tst_maptestgui.moc"
