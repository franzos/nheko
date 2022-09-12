import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.12
import MatrixClient 1.0
import QmlInterface 1.0
import GlobalObject 1.0
import CursorShape 1.0
import "regex"
import "ui"

Dialog {
    standardButtons: StandardButton.Ok | StandardButton.Cancel

    Column {
        width: parent.width
        spacing: 0
        TextField {
            id: userIDField
            width:parent.width
            placeholderText: qsTr("User ID: " + QmlInterface.defaultUserIdFormat())
            onTextChanged: {
                var list = MatrixClient.knownUsers(text)
                memberList.model.clear()
                for(var i = 0; i < list.length; i++){
                    memberList.model.append({
                        userId     : list[i].userId,
                        displayName: list[i].displayName,
                        avatarUrl  : list[i].avatarUrl
                    })
                }
            }
        }
        ListView {
            id: memberList
            width: parent.width
            height: 200
            clip: true
            // boundsBehavior: Flickable.StopAtBounds
            model: ListModel {}
            visible: (model.count?true:false)
            ScrollBar.vertical: ScrollBar {}

            delegate: ItemDelegate {
                id: del

                onClicked: {
                    userIDField.text = model.userId
                }
                padding: 8 //Nheko.paddingMedium
                width: ListView.view.width
                height: memberLayout.implicitHeight + 4 * 2 //Nheko.paddingSmall
                hoverEnabled: true
                RowLayout {
                    id: memberLayout

                    spacing: 8 //Nheko.paddingMedium
                    anchors.centerIn: parent
                    width: parent.width - 4 * 2 //Nheko.paddingSmall

                    Avatar {
                        id: avatar

                        width: GlobalObject.avatarSize
                        height: GlobalObject.avatarSize
                        userid: model.userId
                        url: model.avatarUrl.replace("mxc://", "image://MxcImage/")
                        displayName: model.displayName
                        enabled: false
                    }

                    ColumnLayout {
                        spacing: 4//Nheko.paddingSmall

                        ElidedLabel {
                            fullText: model.displayName
                            font.pixelSize: fontMetrics.font.pixelSize
                            elideWidth: del.width - 8 * 2 -avatar.width  //Nheko.paddingMedium
                        }

                        ElidedLabel {
                            fullText: model.userId
                            color: del.hovered ? GlobalObject.colors.brightText : GlobalObject.colors.buttonText
                            font.pixelSize: Math.ceil(fontMetrics.font.pixelSize * 0.9)
                            elideWidth: del.width - 8 * 2 - avatar.width //Nheko.paddingMedium
                        }

                    }
                }

                CursorShape {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }

            }
        } 
    }
    onButtonClicked: {
        if (clickedButton==StandardButton.Ok) {
            MatrixClient.onUserInfoLoaded.connect(gotoInvite)
            MatrixClient.onUserInfoLoadingFailed.connect(disconnectSignals)
            MatrixClient.userInformation(userIDField.text)
        } else if (clickedButton==StandardButton.Cancel) {
            userIDField.text = ""
        }
    }

    function disconnectSignals(msg){
        MatrixClient.onUserInfoLoaded.disconnect(gotoInvite)
        MatrixClient.onUserInfoLoadingFailed.disconnect(disconnectSignals)
    }

    function gotoInvite(userinformation){
        MatrixClient.startChat(userinformation.userId, false)
        disconnectSignals("")
        userIDField.text = ""
        close()
    }
}