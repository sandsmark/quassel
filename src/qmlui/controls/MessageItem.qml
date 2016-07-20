import QtQuick 2.4
import QtQuick.Controls 1.4

Item {
    property string sender
    property string message
    property date timestamp
    property bool lastSeen
    property bool previousSibling
    property bool nextSibling
    property bool highlighted
    property bool action

    property real padding: 5
    height: shape.height + padding

    Rectangle {
        id: shape
        x: padding
        y: previousSibling ? 0 : padding
        width: parent.width - padding * 2
        height: y + column.height
        radius: 5
        color: Qt.rgba(1, 1, 1, 0.3)
//        color: highlighted ? "orange" : "transparent"

        Grid {
            id: column
            anchors.centerIn: parent
            width: parent.width - padding * 2
            spacing: padding
            columns: 3
            rows: 1

            Label {
                text: Qt.formatDateTime(timestamp)
                opacity: previousSibling ? 0 : 1
            }

            Label {
                opacity: previousSibling ? 0 : 1
                text: sender.split('!')[0]
                textFormat: Text.PlainText
                font.weight: Font.Bold
                color: {
                    var hash = 5381
                    for (var i = 0; i < text.length; i++) {
                        hash = ((hash << 5) + hash) + text.charCodeAt(i)
                    }
                    return [
                                '#8179d7',
                                '#f2749a',
                                '#7ec455',
                                '#f3c34a',
                                '#5b9dd8',
                                '#62b8cd',
                                '#ed8b4a',
                                '#d95848'
                            ][Math.abs(hash) % 8]
                }
//                Rectangle {
//                    anchors.fill: parent
//                    color: "green"
//                    opacity: 0.1
//                }
            }

            StyledLabel {
                id: messageLabel
                text: message
                //                color: highlighted ? theme.palette.highlighted.foregroundText : theme.palette.normal.foregroundText
                width: column.width
//                Rectangle {
//                    anchors.fill: parent
//                    color: "red"
//                    opacity: 0.1
//                }
            }
        }
    }
//                Rectangle {
//                    anchors.fill: parent
//                    color: "yellow"
//                    opacity: 0.1
//                }
}
