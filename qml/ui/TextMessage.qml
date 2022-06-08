import ".."
import QtQuick.Controls 2.3
import GlobalObject 1.0
import CursorShape 1.0

MatrixText {
    required property string body
    required property bool isOnlyEmoji
    required property bool isReply
    required property string formatted
    property string copyText: selectedText ? getText(selectionStart, selectionEnd) : body
    property int metadataWidth
    property bool fitsMetadata: positionAt(width,height-4) == positionAt(width-metadataWidth-10, height-4)

    // table border-collapse doesn't seem to work
    text: "
    <style type=\"text/css\">
    a { color:" + GlobalObject.colors.link + ";}
    code { background-color: " + GlobalObject.colors.alternateBase + ";}
    table {
        border-width: 1px;
        border-collapse: collapse;
        border-style: solid;
    }
    table th,
    table td {
        bgcolor: " + GlobalObject.colors.alternateBase + ";
        border-collapse: collapse;
        border: 1px solid " + GlobalObject.colors.text + ";
    }
    blockquote { margin-left: 1em; }
    </style>
    " + formatted.replace(/<pre>/g, "<pre style='white-space: pre-wrap; background-color: " + GlobalObject.colors.alternateBase + "'>").replace(/<del>/g, "<s>").replace(/<\/del>/g, "</s>").replace(/<strike>/g, "<s>").replace(/<\/strike>/g, "</s>")
    width: parent.width
    height: implicitHeight//isReply ? Math.round(Math.min(timelineView.height / 8, implicitHeight)) : implicitHeight
    clip: isReply
    selectByMouse: true//!Settings.mobileMode && !isReply
    // font.pointSize: (Settings.enlargeEmojiOnlyMessages && isOnlyEmoji > 0 && isOnlyEmoji < 4) ? Settings.fontSize * 3 : Settings.fontSize

    CursorShape {
        enabled: isReply
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
    }

}
