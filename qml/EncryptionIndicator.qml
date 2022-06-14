import QtQuick 2.12
import QtQuick.Controls 2.1
import QtQuick.Window 2.15
import GlobalObject 1.0
import Crypto 1.0

Image {
    id: stateImg

    property bool encrypted: false
    property int trust: Crypto.Unverified

    property string sourceUrl: {
        if (!encrypted)
        return "image://colorimage/:/images/shield-filled-cross.svg?";

        switch (trust) {
            case Crypto.Verified:
                return "image://colorimage/:/images/shield-filled-checkmark.svg?";
            case Crypto.TOFU:
                return "image://colorimage/:/images/shield-filled.svg?";
            case Crypto.Unverified:
                return "image://colorimage/:/images/shield-filled-exclamation-mark.svg?";
            default:
                return "image://colorimage/:/images/shield-filled-cross.svg?";
        }
    }

    width: 16
    height: 16
    sourceSize.height: height * Screen.devicePixelRatio
    sourceSize.width: width * Screen.devicePixelRatio
    source: {
        if (encrypted) {
            switch (trust) {
            case Crypto.Verified:
                return sourceUrl + "green";
            case Crypto.TOFU:
                return sourceUrl + GlobalObject.colors.buttonText;
            default:
                return sourceUrl + GlobalObject.theme.error;
            }
        } else {
            return sourceUrl + GlobalObject.theme.error;
        }
    }
    ToolTip.visible: ma.hovered
    ToolTip.text: {
        if (!encrypted)
            return qsTr("This message is not encrypted!");

        switch (trust) {
        case Crypto.Verified:
            return qsTr("Encrypted by a verified device");
        case Crypto.TOFU:
            return qsTr("Encrypted by an unverified device, but you have trusted that user so far.");
        default:
            return qsTr("Encrypted by an unverified device or the key is from an untrusted source like the key backup.");
        }
    }

    HoverHandler {
        id: ma
    }

}
