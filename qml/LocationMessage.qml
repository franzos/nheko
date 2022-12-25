import QtQuick 2.15
import QtQuick.Controls 2.3
import QtLocation 5.15
import QtPositioning 5.15
import GlobalObject 1.0

AbstractButton {
    id: lm
    required property int type
    required property int originalWidth
    required property double proportionalHeight
    required property string blurhash
    required property string body
    required property bool isReply
    required property string eventId
    required property string geoUri
    property double divisor: isReply ? 5 : 3
    property int tempWidth: originalWidth < 1? 400: originalWidth

    implicitWidth: Math.round(tempWidth*Math.min((timeline.height/divisor)/(tempWidth*proportionalHeight), 1))
    width: Math.min(parent.width,implicitWidth)
    height: width*proportionalHeight
    hoverEnabled: true

    property int metadataWidth
    property bool fitsMetadata: (parent.width - width) > metadataWidth+4
    
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

        Component.onCompleted: {
            circle = Qt.createQmlObject('import QtLocation 5.15; MapCircle {}', lm)
            circle.center = mapview.center
            circle.radius = 30
            circle.color = 'green'
            circle.border.width = 0
            mapview.addMapItem(circle)
        }
        
    }

    Component.onCompleted: {
        var coord = geoUri.split(":")
        if(coord.length > 1){
            var lat_long_arr = coord[1].split(",")
            if(lat_long_arr.length > 1) {
                var lat = Number(lat_long_arr[0])
                var lon = Number(lat_long_arr[1])
                mapview.center = QtPositioning.coordinate(lat,lon)
            }
        }
    }
}