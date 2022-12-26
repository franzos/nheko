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
    property double latitude
    property double longtitude

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
        center: QtPositioning.coordinate(latitude,longtitude)

        MapCircle {
            center: QtPositioning.coordinate(latitude,longtitude)
            radius: 10
            color: 'green'
            border.width: 0
        }
        MouseArea {
            id: mouseArea
            property variant lastCoordinate
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed : {
                GlobalObject.openLink(geoUri + "?q=" + latitude + "," + longtitude + "(" + body + ")")
            }
        }
    }

    Component.onCompleted: {
        var coord = geoUri.split(":")
        if(coord.length > 1){
            var lat_long_arr = coord[1].split(",")
            if(lat_long_arr.length > 1) {
                latitude = Number(lat_long_arr[0])
                longtitude = Number(lat_long_arr[1])
                
            }
        }
    }
}
