import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import MatrixClient 1.0
import QmlInterface 1.0
import "regex"

Dialog {
    signal userAdded(string userid);

    TextField {
        id: userIDField
        width:parent.width
        validator: UserIDRegex{}
        placeholderText: qsTr("User ID: " + QmlInterface.defaultUserIdFormat())
    }    

    footer: DialogButtonBox{
        Button {
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
        Button {
            text: qsTr("Ok")
            onClicked: {
                MatrixClient.onUserInfoLoaded.connect(gotoInvite)
                MatrixClient.onUserInfoLoadingFailed.connect(disconnectSignals)
                MatrixClient.userInformation(userIDField.text)
            }
        }
    }

    onRejected: {
        userIDField.text = ""
    }

    function disconnectSignals(msg){
        MatrixClient.onUserInfoLoaded.disconnect(gotoInvite)
        MatrixClient.onUserInfoLoadingFailed.disconnect(disconnectSignals)
    }

    function gotoInvite(userinformation){
        userAdded(userinformation.userId)
        disconnectSignals("")
        userIDField.text = ""
        close()
    }
}