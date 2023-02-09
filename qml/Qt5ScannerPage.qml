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

  property var mx_id;

  Camera {
    id: camera

    focus {
      focusMode: CameraFocus.FocusContinuous
      focusPointMode: CameraFocus.FocusPointAuto
    }
  }

  VideoOutput {
    id: videoOutput
    anchors.fill: parent
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

  SBarcodeScanner {
    id: barcodeScanner
    // you can adjust capture rect (scan area) ne changing these Qt.rect() parameters
    captureRect: videoOutput.mapRectToSource(videoOutput.mapNormalizedRectToItem(Qt.rect(0.25, 0.25, 0.5, 0.5)))
    onCapturedChanged: {
      active = false
      console.log("captured: " + captured)
      startChatButton.visible=false
      mx_id = ""
      try{
        var JsonObject = JSON.parse(captured)
        if(JsonObject.mx_id) {
          mx_id = JsonObject.mx_id
          startChatButton.visible=true
          scanResultText.text="Do you want to start direct chat with \"" + mx_id + "\"?"
        } else {
          scanResultText.text="Invalid Matrix ID QR Code!"
        }
      } catch(err) {
        scanResultText.text="Invalid Matrix ID QR Code!"
        console.log(err)
      }
    }
  }
  
  Rectangle {
    id: resultScreen
    anchors.fill: parent
    visible: !barcodeScanner.active
    Column {
      anchors.centerIn: parent
      width: parent.width
      spacing: 20
      Label {
        id: scanResultText
        width: parent.width - 20
        anchors.horizontalCenter: parent.horizontalCenter
        wrapMode: Text.Wrap
        color: GlobalObject.colors.text
      }
    }
  }
  footer: DialogButtonBox {
    id: footerDialog
    visible: !barcodeScanner.active
    Button {
      id: scanButton
      text: qsTr("Scan again")
      DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
      onClicked: {
        barcodeScanner.active = true
      }
    }

    Button {
      id: startChatButton
      text: qsTr("Start Chat")
      DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
      onClicked: {
        if(mx_id !== ''){
          MatrixClient.startChat(mx_id, false)
          close()
        }
      }
    }
    
    background: Rectangle {
        anchors.fill: parent
        color: GlobalObject.colors.window
    }
  }
}
