import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import com.scythestudio.scodes 1.0
import Settings 1.0

CustomApplicationWindow {
  id: qrCodeWindow
  minimumWidth: 340
  minimumHeight: 450
  width: 450
  height: 680
  modality: Qt.NonModal
  flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
  title: qsTr("My QR code")

  SBarcodeGenerator {
    id: barcodeGenerator

    onGenerationFinished: function (error) {
      if (error === "") {
        console.log(barcodeGenerator.filePath)
        image.source = "file:///" + barcodeGenerator.filePath
      } else {
        generateLabel.text = error
        generatePopup.open()
      }
    }
  }
  
  Component.onCompleted: {
    image.source = ""
    barcodeGenerator.setFormat("QRCode")
    barcodeGenerator.generate("{\"mx_id\": \"" + Settings.userId + "\"}")
  }

  Rectangle {
    id: dashboard
    anchors.fill: parent
    Image {
      id: image
      width: parent.width
      height: image.width
      anchors {
        left: parent.left
        right: parent.right
        verticalCenter: parent.verticalCenter
      }
      cache: false
    }

    Popup {
      id: generatePopup
      anchors.centerIn: parent
      dim: true
      modal: true

      Label {
        id: generateLabel
        anchors.centerIn: parent
      }
    }
  }
}
