import QtQuick 2.0
import QtTest 1.0
import Esri.ArcGISRuntime 100.10

TestCase {
    name: "MapTestQml"
    id: mapTestQml
    property url m_webMapUrl: "https://www.arcgis.com/home/webmap/viewer.html?webmap=56bb3556bc5542cfa217348b09c80efb"

    function test_1_simple_property() {
        const map = ArcGISRuntimeEnvironment.createObject("Map");
        const layer = ArcGISRuntimeEnvironment.createObject("OpenStreetMapLayer");
        map.operationalLayers.append(layer);

        compare(map.operationalLayers.count, 1);
        compare(map.operationalLayers.get(0).layerType, Enums.LayerTypeOpenStreetMapLayer);
    }

    function test_2_async_method() {
        const map = ArcGISRuntimeEnvironment.createObject("Map", {initUrl: m_webMapUrl});

        map.load();

        tryCompare(map, "loadStatus", Enums.LoadStatusLoaded);
    }

    function test_3_async_method2() {
        const map = ArcGISRuntimeEnvironment.createObject("Map", {initUrl: m_webMapUrl});

        let loadStatusFromSignal = Enums.LoadStatusUnknown;

        map.loadStatusChanged.connect(function() {
            loadStatusFromSignal = map.loadStatus;
        });

        map.load();

        tryVerify(function() { return loadStatusFromSignal === Enums.LoadStatusLoaded; });
    }

    Component {
        id: test_4_map_component
        Map {
            initUrl: m_webMapUrl
        }
    }

    function test_4_advanced_techniques() {
        const map = createTemporaryObject(test_4_map_component, mapTestQml);

        map.load();

        tryCompare(map, "loadStatus", Enums.LoadStatusLoaded);
    }

    Component {
        id: test_5_map_component
        Map { }
    }

    function test_5_advanced_techniques2() {
        const map = createTemporaryObject(test_5_map_component, mapTestQml, {initUrl: m_webMapUrl});

        map.load();

        tryCompare(map, "loadStatus", Enums.LoadStatusLoaded);
    }

    function test_6_with_failure() {
        const map = ArcGISRuntimeEnvironment.createObject("Map", {initUrl: m_webMapUrl});

        map.load();

        tryCompare(map, "loadStatus", Enums.LoadStatusLoaded);

        compare(map.operationalLayers.count, -1);
    }

    function test_7_data_driven_test_data() {
        return [
            {
                tag: "web map url",
                url: "https://www.arcgis.com/home/webmap/viewer.html?webmap=56bb3556bc5542cfa217348b09c80efb"
            },
            {
                tag: "item url",
                url: "https://www.arcgis.com/home/item.html?id=56bb3556bc5542cfa217348b09c80efb"
            },
            {
                tag: "data sharing url",
                url: "https://www.arcgis.com/sharing/rest/content/items/56bb3556bc5542cfa217348b09c80efb/data"
            }
        ];
    }

    function test_7_data_driven_test(data) {
        const map = ArcGISRuntimeEnvironment.createObject("Map", {initUrl: data.url});

        map.load();

        tryCompare(map, "loadStatus", Enums.LoadStatusLoaded);
    }
}
