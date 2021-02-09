import QtQuick 2.3
import QtQuick.Window 2.0
import QtTest 1.0
import Esri.ArcGISRuntime 100.10

Item {
    id: root

    Window {
        id: testWindow
        width: 600
        height: 800
        visible: true

        Component.onCompleted: {
            show();
            raise();
        }

        MapView {
            id: mapView
            anchors.fill: parent
        }

        TestCase {
            id: simpleGuiTest
            name: "simpleGuiTest"
            when: testWindow.visible

            function cleanup() {
                mapView.wrapAroundMode = Enums.WrapAroundModeEnabledWhenSupported;
            }

            function test_1_wraparound() {
                mapView.wrapAroundMode = Enums.WrapAroundModeEnabledWhenSupported;
                mapView.map = ArcGISRuntimeEnvironment.createObject("Map");
                mapView.map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreets");

                // wait for the map to load
                tryCompare(mapView.map, "loadStatus", Enums.LoadStatusLoaded);

                // set a viewpoint at the edge of the wraparound boundary
                const envelope = ArcGISRuntimeEnvironment.createObject("Envelope",
                                                                       {xMin: -20200191.801510714,
                                                                        yMin: 4284.2465115027735,
                                                                        xMax: -19459110.135313917,
                                                                        yMax: 983824.64175089251});
                const viewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: envelope});
                mapView.setViewpointAndSeconds(viewpoint, 2.5);

                // now wait for the view's draw status to be complete
                tryCompare(mapView, "drawStatus", Enums.DrawStatusCompleted);

                // advanced testing - check pixel data on left-most pixels to see if
                // basemap with blue ocean was rendered

                // this is not needed for the test, it is only to allow a visual inspection
                // for demonstration purposes
                wait(3000);
            }

            function test_2_no_wraparound() {
                mapView.wrapAroundMode = Enums.WrapAroundModeDisabled;
                mapView.map = ArcGISRuntimeEnvironment.createObject("Map");
                mapView.map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapStreets");

                // wait for the map to load
                tryCompare(mapView.map, "loadStatus", Enums.LoadStatusLoaded);

                // set a viewpoint at the edge of the wraparound boundary
                const envelope = ArcGISRuntimeEnvironment.createObject("Envelope",
                                                                       {xMin: -20200191.801510714,
                                                                        yMin: 4284.2465115027735,
                                                                        xMax: -19459110.135313917,
                                                                        yMax: 983824.64175089251});
                const viewpoint = ArcGISRuntimeEnvironment.createObject("ViewpointExtent", {extent: envelope});
                mapView.setViewpointAndSeconds(viewpoint, 2.5);

                // now wait for the view's draw status to be complete
                tryCompare(mapView, "drawStatus", Enums.DrawStatusCompleted);

                // advanced testing - check pixel data on left-most pixels to see if
                // gray map background is displayed since wraparound was disabled

                // this is not needed for the test, it is only to allow a visual inspection
                // for demonstration purposes
                wait(3000);
            }

            function test_3_identify_graphic() {
                mapView.map = ArcGISRuntimeEnvironment.createObject("Map");
                mapView.map.basemap = ArcGISRuntimeEnvironment.createObject("BasemapNationalGeographic");

                // wait for the map to load
                tryCompare(mapView.map, "loadStatus", Enums.LoadStatusLoaded);

                // get the center point of the window
                const screenCenterX = mapView.widthInPixels / 2;
                const screenCenterY = mapView.heightInPixels / 2;

                // convert the screen coordinates to map coordinates
                const mapCoordinatesScreenCenter = mapView.screenToLocation(screenCenterX, screenCenterY);

                // add a graphics overlay and a graphic
                const symbol = ArcGISRuntimeEnvironment.createObject("SimpleMarkerSymbol", {color: "red",
                                                                     size: 64,
                                                                     style: Enums.SimpleMarkerSymbolStyleSquare});
                const graphic = ArcGISRuntimeEnvironment.createObject("Graphic", {geometry: mapCoordinatesScreenCenter,
                                                                                  symbol: symbol});

                const overlay = ArcGISRuntimeEnvironment.createObject("GraphicsOverlay");

                mapView.graphicsOverlays.append(overlay);

                overlay.graphics.append(graphic);

                // now wait for the view's draw status to be complete
                tryCompare(mapView, "drawStatus", Enums.DrawStatusCompleted);

                // connect to the signal for identifying graphics
                mapView.identifyGraphicsOverlayStatusChanged.connect(function(){
                    if (mapView.identifyGraphicsOverlayStatus !== Enums.TaskStatusCompleted)
                        return;

                    if (mapView.identifyGraphicsOverlayResult && mapView.identifyGraphicsOverlayResult.graphics.length > 0)
                        mapView.identifyGraphicsOverlayResult.graphics[0].selected = true;
                });

                // let's pause here for 3 seconds so the graphic can be seen in it's unselected state
                wait(3000);

                // now identify the graphic in the center position on the screen
                mapView.identifyGraphicsOverlay(overlay, screenCenterX, screenCenterY, 5, false);

                // after we process the identify, we will select the graphic
                tryCompare(graphic, "selected", true);

                // this is not needed for the test, it is only to allow a visual inspection
                // for demonstration purposes
                wait(3000);
            }
        }
    }
}
