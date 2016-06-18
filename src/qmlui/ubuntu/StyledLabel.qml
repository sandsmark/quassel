import QtQuick 2.4
import Ubuntu.Components 1.3

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
        text: parent.text.replace(/[><&]/g, function(s) {
                return {
                    '>': '&gt;',
                    '<': '&lt;',
                    '&': '&amp;',
                    '"': '&quot;',
                }[s]
            })
            .replace(/(https?:\/\/[^\s]+)/g, '<a href="$1">$1</a>')
        textFormat: Text.StyledText
        wrapMode: Text.Wrap
        elide: wrapMode == Text.NoWrap ? Text.ElideRight : Text.ElideNone
        linkColor: UbuntuColors.orange
        onLinkActivated: Qt.openUrlExternally(link)
    }
}
