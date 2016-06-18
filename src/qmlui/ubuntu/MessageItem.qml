import QtQuick 2.4
import Ubuntu.Components 1.3

ListItem {
    property string sender
    property string message
    property date timestamp
    property bool lastSeen
    property bool previousSibling
    property bool nextSibling
    property bool highlighted
    property bool action

    property real padding: units.gu(1)
    divider.visible: lastSeen
    divider.colorFrom: UbuntuColors.orange
    height: shape.height + padding

    UbuntuShape {
        id: shape
        x: padding
        y: previousSibling ? 0 : padding
        width: parent.width - padding * 2
        height: y + column.height
        aspect: UbuntuShape.Flat
        color: highlighted ? theme.palette.highlighted.foreground : theme.palette.normal.foreground

        Grid {
            id: column
            anchors.centerIn: parent
            width: parent.width - padding * 2
            columns: action ? 2 : 1
            rows: action ? 1 : 2
            spacing: padding

            Row {
                spacing: padding
                visible: !previousSibling

                Label {
                    text: i18n.relativeDateTime(timestamp)
                    textFormat: Text.PlainText
                    color: theme.palette.disabled.foregroundText
                }

                Label {
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
            }

            StyledLabel {
                id: messageLabel
                text: message
                color: highlighted ? theme.palette.highlighted.foregroundText : theme.palette.normal.foregroundText
                width: column.width
            }
        }
    }
}
