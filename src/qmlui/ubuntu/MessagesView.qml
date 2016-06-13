import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    id: messagesPage
    property alias bufferId: messagesModel.bufferId
    property alias networkId: messagesModel.networkId
    property alias nick: nickLabel.text

    header: PageHeader {
        id: messagesHeader
        title: messagesModel.bufferName
        subtitle: messagesModel.topic
        flickable: messagesList
        clip: true
    }

    UbuntuListView {
        id: messagesList
        anchors {
            top: parent.top
            bottom: messageInput.top
        }
        width: parent.width
        clip: true
        highlight: null
        model: MessageModel {
            id: messagesModel
        }
        delegate: ListItem {
            height: messageItemLayout.height
            divider.visible: model.divider || lastSeen
            divider.colorFrom: lastSeen ? UbuntuColors.orange : theme.palette.normal.base
            property bool lastSeen: messagesModel.lastSeenMsgId == model.msgId
            ListItemLayout {
                id: messageItemLayout
                subtitle.text: model.divider ? '<b>%2</b> %1'.arg(i18n.relativeDateTime(timestamp)).arg(sender.split('!')[0]) : ''
                title.text: message.replace('>', '&gt;').replace('<', '&lt;').replace('&', '&amp;').replace(/(https?:\/\/[^\s]+)/g, '<a href="$1">$1</a>')
                title.textFormat: Text.StyledText
                title.linkColor: UbuntuColors.orange
                title.onLinkActivated: Qt.openUrlExternally(link)
                title.maximumLineCount: 0
            }
        }

        ShortcutBubble {
            anchors.right: parent.right
            y: height + messagesHeader.y + messagesHeader.height
            anchors.margins: width
            shortcut: '▲'
            visible: messageInput.activeFocus && shiftPressed
        }

        ShortcutBubble {
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: width
            shortcut: '▼'
            visible: messageInput.activeFocus && shiftPressed
        }
    }

    TextField {
        id: messageInput
        anchors {
            bottom: messagesPage.bottom
            topMargin: units.gu(0.5)
            bottomMargin: units.gu(0.5)
        }
        width: parent.width
        enabled: false // FIXME: messagesModel.connected
        primaryItem: Label {
            id: nickLabel
            text: messagesModel.nick
            enabled: false
        }
        property real scrollOffset: height
        Keys.onPressed: {
            if (!(event.modifiers & Qt.ShiftModifier))
                return
            switch (event.key) {
            case Qt.Key_Down:
                messagesList.contentY += scrollOffset
                break
            case Qt.Key_Up:
                messagesList.contentY -= scrollOffset
                break
            default:
                return
            }
            messagesList.returnToBounds()
            event.accepted = true
        }
        action: Action {
            id: messageInputFocusAction
            shortcut: 'Ctrl+L'
            onTriggered: messageInput.forceActiveFocus(Qt.TabFocusReason)
        }
        secondaryItem: ShortcutBubble {
            action: messageInputFocusAction
            visible: controlPressed
        }
    }
}
