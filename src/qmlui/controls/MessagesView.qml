import QtQuick 2.4
import QtQuick.Controls 2.0
import Quassel 0.1

Item {
    property alias bufferId: messagesModel.bufferId

//    TableView {
//        anchors {
//            top: parent.top
//            bottom: messageInput.top
//            left: parent.left
//            right: parent.right
//        }

//        id: messagesList
//        clip: true
//        model: MessageModel {
//            id: messagesModel
//            onBacklogReceived:{
//                messagesList.positionViewAtRow(rowCount -1, ListView.Contain)
//            }
//            onBufferIdChanged: messagesList.positionViewAtRow(rowCount -1, ListView.Contain)
//        }
//        TableViewColumn {
//            role: "timestamp"
//            title: "Timestamp"
//            width: 200
//        }
//        TableViewColumn {
//            role: "sender"
//            title: "Sender"
//            width: 200
//        }
//        TableViewColumn {
//            role: "message"
//            title: "Message"
//        }

//        Component.onCompleted: messagesList.positionViewAtRow(rowCount -1, ListView.Contain)
//    }


//    Frame {
        ListView {
            id: messagesList
            clip: true
            anchors {
                top: parent.top
                bottom: messageInput.top
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
                lastSeen: messagesModel.lastSeenMsgId == model.msgId
                previousSibling: model.previousSibling
                nextSibling: model.nextSibling
                message: model.styledMessage
                timestamp: model.timestamp
                highlighted: model.highlight || model.self
                action: model.action
            }
        }
//    }

    Label {
        id: nickLabel
        text: messagesModel.nick
        anchors {
            left: parent.left
            bottom: parent.bottom
            top: messageInput.top
        }
        verticalAlignment: Text.AlignVCenter
    }

    TextField {
        id: messageInput
        anchors {
            bottom: parent.bottom
            right: parent.right
            left: nickLabel.right
            leftMargin: 5
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
