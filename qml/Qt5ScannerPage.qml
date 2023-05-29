import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12
import QtQuick.Layouts 1.12
import MatrixClient 1.0
import GlobalObject 1.0
import com.scythestudio.scodes 1.0


CustomApplicationWindow {
  id: scannerpage
  minimumWidth: 340
  minimumHeight: 450
  width: 450
  height: 680
  modality: Qt.NonModal
  flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
  title: qsTr("Scan QR code ...")

  Camera {
    id: camera

    focus {
      focusMode: CameraFocus.FocusContinuous
      focusPointMode: CameraFocus.FocusPointAuto
    }
  }

  ColumnLayout {
    anchors.fill: parent

    VideoOutput {
      id: videoOutput
      // anchors.fill: parent
      Layout.fillHeight: true
      Layout.fillWidth: true
      source: camera
      autoOrientation: true
      fillMode: VideoOutput.PreserveAspectCrop
      // add barcodeScanner to videoOutput's filters to enable catching barcodes
      filters: [barcodeScanner]
      onSourceRectChanged: {
        barcodeScanner.captureRect = videoOutput.mapRectToSource(videoOutput.mapNormalizedRectToItem(Qt.rect(0.25, 0.25, 0.5, 0.5)))
      }

      Qt5ScannerOverlay {
        id: scannerOverlay
        anchors.fill: parent
        captureRect: videoOutput.mapRectToItem(barcodeScanner.captureRect)
      }

      // used to get camera focus on touched point
      MouseArea {
        id: focusTouchArea
        anchors.fill: parent
        onClicked: {
          camera.focus.customFocusPoint = Qt.point(mouse.x / width,
                                                    mouse.y / height)
          camera.focus.focusMode = CameraFocus.FocusMacro
          camera.focus.focusPointMode = CameraFocus.FocusPointCustom
        }
      }
    }

    Button {
        id: cancelButton
        Layout.fillWidth: true
        onClicked: scannerpage.close()
        text: qsTr("Cancel")
    }
  }

  SBarcodeScanner {
    id: barcodeScanner
    // you can adjust capture rect (scan area) ne changing these Qt.rect() parameters
    captureRect: videoOutput.mapRectToSource(videoOutput.mapNormalizedRectToItem(Qt.rect(0.25, 0.25, 0.5, 0.5)))
    onCapturedChanged: {
      active = false
      try{
        var JsonObject = JSON.parse(captured)
        if(JsonObject.mx_id) {
          var mx_id = JsonObject.mx_id
          console.log("QR Direct chat: " + mx_id)
          MatrixClient.startChat(mx_id, false)
          close()
        } else {
          console.log("Invalid matrix id: " + captured)
          active = true
        }
      } catch(err) {
        active = true
        console.log("Invalid matrix id: " + captured)
        console.log(err)
      }
    }
  }
}
