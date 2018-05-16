import QtQuick 2.4
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Quassel 0.1

Page {
    id: messagesView
    property alias bufferId: messagesModel.bufferId

    property int nickWidth: 200

    header: ToolBar {
        RowLayout {
            ToolButton {
                padding: 5
                text: "Channels"
                onClicked: bufferDrawer.open()
                opacity: bufferDrawer.visible ? 0 : 1
                Behavior on opacity { NumberAnimation { duration: 200 } }
            }
            Item { Layout.fillWidth: true }
        }
    }

    ListView {
        id: messagesList
        clip: true
        anchors {
            top: parent.top
            bottom: messageInput.top
            bottomMargin: 5
            left: parent.left
            right: parent.right
        }
        highlight: null
        model: MessageModel {
            id: messagesModel
            onBacklogReceived:{
                messagesList.positionViewAtEnd()
            }
            onBufferIdChanged: messagesList.positionViewAtEnd()
        }

        Component.onCompleted: positionViewAtEnd()

        delegate: MessageItem {
            width: messagesList.width
            sender: model.sender
            lastSeen: messagesModel.lastSeenMsgId === model.msgId
            previousSibling: model.previousSibling
            nextSibling: model.nextSibling
            message: model.styledMessage
            timestamp: model.timestamp
            highlighted: model.highlight || model.self
            action: model.action
            nickWidth: messagesView.nickWidth
            onActualNickWidthChanged: messagesView.nickWidth = Math.max(messagesView.nickWidth, actualNickWidth)
        }
    }

    Label {
        id: nickLabel
        text: messagesModel.nick
        anchors {
            left: parent.left
            leftMargin: 5
            verticalCenter: messageInput.verticalCenter
        }
        font.bold: true
        verticalAlignment: Text.AlignVCenter
    }

    TextField {
        id: messageInput
        anchors {
            bottom: parent.bottom
            margins: 5
            right: parent.right
            left: nickLabel.right
        }
        enabled: messagesModel.connected
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
    }
}
