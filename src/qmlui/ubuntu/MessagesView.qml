import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    id: messagesPage
    property alias bufferId: messagesModel.bufferId
    property alias networkId: messagesModel.networkId

    header: PageHeader {
        id: messagesHeader
        title: messagesModel.bufferName
        Mouse.forwardTo: [extension]
        extension: HeaderExtensionLabel {
            text: messagesModel.topic
        }
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
        delegate: MessageItem {
            sender: model.sender
            lastSeen: messagesModel.lastSeenMsgId == model.msgId
            previousSibling: model.previousSibling
            nextSibling: model.nextSibling
            message: model.styledMessage
            timestamp: model.timestamp
            highlighted: model.highlight || model.self
            action: model.action
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
        property real padding: units.gu(0.5)
        anchors {
            bottom: messagesPage.bottom
            topMargin: padding
            bottomMargin: padding
        }
        x: padding
        width: parent.width - padding * 2
        enabled: messagesModel.connected
        primaryItem: Label {
            id: nickLabel
            text: messagesModel.nick
            enabled: false
        }
        onAccepted: {
            messagesModel.userInput(text)
            text = ''
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
