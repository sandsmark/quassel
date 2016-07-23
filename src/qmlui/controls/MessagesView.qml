import QtQuick 2.7
import QtQuick.Controls 1.4
import Quassel 0.1

Item {
    property alias bufferId: messagesModel.bufferId
    property alias bufferName: messagesModel.bufferName
    property int padding: 5

    ListView {
        id: messagesList
        clip: true
        anchors {
            top: parent.top
            bottom: messageInput.top
            left: parent.left
            right: parent.right
            margins: padding
        }
        highlight: null
        model: MessageModel {
            id: messagesModel
        }

        header: ProgressBar {
            id: progressBar
            width: parent.width
            indeterminate: true
        }


        // Handle scrolling to end
        onAtYEndChanged: {
            if (flicking) {
                return
            }
            if (countBeforeRequest === 0) {
                positionViewAtEnd()
                return
            }
        }

        property int countBeforeRequest: 0
        onCountChanged: {
            if (countBeforeRequest === 0) {
                return
            }

            positionViewAtIndex(count - countBeforeRequest, ListView.Beginning)
        }
        onFlickEnded: {
            if (atYBeginning) {
                cancelFlick()
                countBeforeRequest = count
                messagesModel.requestMoreBacklog()
            } else if (atYEnd) {
                countBeforeRequest = 0
            }
        }

        delegate: MessageItem {
            width: messagesList.width
            sender: model.sender
            lastSeen: messagesModel.lastSeenMsgId == model.msgId
            previousSibling: model.previousSibling
            nextSibling: model.nextSibling
            message: model.styledMessage
            timestamp: model.timestamp
            highlighted: model.highlight || model.self
            action: model.action
            query: model.query
        }

        Button {
            visible: !parent.atYEnd
            anchors {
                bottom: parent.bottom
                right: parent.right
            }
            text: "▼"
            height: width
            onClicked:{
                messagesList.countBeforeRequest = 0
                messagesList.positionViewAtEnd()
            }
        }
    }

    Label {
        id: nickLabel
        text: messagesModel.nick
        anchors {
            left: parent.left
            bottom: parent.bottom
            top: messageInput.top
            margins: padding
        }
        verticalAlignment: Text.AlignVCenter
    }

    TextField {
        id: messageInput
        anchors {
            bottom: parent.bottom
            right: parent.right
            left: nickLabel.right
            margins: padding
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
