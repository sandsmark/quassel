import QtQuick 2.4
import QtQuick.Controls 1.4

FocusScope {
    implicitHeight: label.height
    property string text
    property alias color: label.color
    property alias maximumLineCount: label.maximumLineCount
    property alias wrapMode: label.wrapMode

    Label {
        id: label
        height: contentHeight
        width: parent.width
        text: parent.text
        textFormat: Text.StyledText
        wrapMode: Text.Wrap
        elide: wrapMode == Text.NoWrap ? Text.ElideRight : Text.ElideNone
        onLinkActivated: Qt.openUrlExternally(link)
    }
}
