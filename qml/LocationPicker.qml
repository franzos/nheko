import QtQuick 2.15
import QtQuick.Controls 2.3
import QtLocation 5.15
import QtPositioning 5.15
import GlobalObject 1.0
import TimelineModel 1.0

CustomApplicationWindow {
    id: roomMembersRoot
    property double latitude: 51.477928
    property double longtitude: -0.001545
    property bool currentLocationFound: false
    property var room: timelineModel
    Plugin {
        id: mapPlugin
        name: "osm"
        // "mapboxgl", "esri", ...
        // PluginParameter { name: "osm.mapping.offline.directory"; value: "//offlinemaps directory" }
    }

    Map {
        id: mapview
        property MapCircle circle

        anchors.fill: parent
        plugin: mapPlugin
        zoomLevel: maximumZoomLevel*4/5
        center: QtPositioning.coordinate(latitude, longtitude)

        MapCircle {
            id: mapCircle
            center: QtPositioning.coordinate(latitude,longtitude)
            radius: 10
            color: 'green'
            border.width: 0
        }
    }

    PositionSource {
        id: src
        updateInterval: 1000
        active: true

        onPositionChanged: {
            var coord = src.position.coordinate;
            if(!currentLocationFound) {
                currentLocationFound = true
                mapview.center = coord
                mapCircle.center = coord
            }
        }
    }

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
        onAccepted: {
            var coord = mapCircle.center;
            room.input.location(coord.latitude, coord.longitude)
            close()
        }
        onRejected: {
            close()
        }
        background: Rectangle {
            anchors.fill: parent
            color: GlobalObject.colors.window
        }
    }
}
