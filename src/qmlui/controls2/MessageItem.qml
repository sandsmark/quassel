import QtQuick 2.4
import QtQuick.Controls 2.0

Item {
    id: messageItem
    property string sender
    property alias message: messageLabel.text
    property date timestamp
    property bool lastSeen
    property bool previousSibling
    property bool nextSibling
    property bool highlighted
    property bool action
    property bool query

    property alias actualNickWidth: nickLabel.width
    property alias nickWidth: nickLabelContainer.width

    property real padding: 2
    height: column.height + padding

    Row {
        id: column
        x: padding
        y: previousSibling ? 0 : padding
        width: parent.width - padding * 2
        spacing: padding

        Label {
            id: timestamp
            text: Qt.formatDateTime(messageItem.timestamp, "hh:mm")
            opacity: previousSibling ? 0 : 1
        }

        Item {
            id: nickLabelContainer

            height: nickLabel.height
            width: messageItem.nickWidth

            Label {
                id: nickLabel
                anchors.right: parent.right
                opacity: (!action && previousSibling) ? 0 : 1
                text: sender.split('!')[0]
                textFormat: Text.PlainText
                font.weight: Font.Bold
                onWidthChanged: if (width > nickWidth) {
                    messagesView.nickWidth = width + padding * 2
                }

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
        }

        StyledLabel {
            id: messageLabel
            opacity: action ? 0.3 : 1
            width: column.width - timestamp.width - nickLabel.width - column.padding * 6
        }
    }
}
