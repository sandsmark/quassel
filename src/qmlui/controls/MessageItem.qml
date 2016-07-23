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
    property bool query

    property real padding: 5
    height: column.height + padding

    Row {
        id: column
        x: padding
        y: previousSibling ? 0 : padding
        width: parent.width - padding * 2
        spacing: padding

        Label {
            id: timestamp
            text: Qt.formatDateTime(timestamp, "hh:mm")
            opacity: previousSibling ? 0 : 1
        }

        Label {
            id: nickLabel
            opacity: (!action && previousSibling) ? 0 : 1
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
        }

        StyledLabel {
            id: messageLabel
            text: message
            opacity: action ? 0.3 : 1
            width: column.width - timestamp.width - nickLabel.width - column.padding * 6
        }
    }
}
