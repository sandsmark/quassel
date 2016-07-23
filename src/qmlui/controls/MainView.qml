import QtQuick 2.4
import QtQml 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import Quassel 0.1

ApplicationWindow {
    id: window
    width: 1920
    height: 1080
    visible: true
    visibility: "Maximized"

    title: qsTr('Quassel IRC')

    property bool loggedIn: false
    onLoggedInChanged: {
        if (loggedIn) {
            stackView.push(bufferComponent)
        } else {
            stackView.clear()
            stackView.push(loginComponent)
            statusText.text = "Disconnected"
        }
    }

    Component {
        id: loginComponent
        LoginView {
            id: loginView
            onLoggedInChanged: {
                window.loggedIn = loggedIn
            }
        }
    }

    StackView {
        id: stackView
        width: parent.width
        height: parent.height
        initialItem: loginComponent
    }

    property int currentBuffer: 0

    Component {
        id: bufferComponent
        Item {
        TreeView {
            id: networksList
            model: BufferModel
            anchors.fill: parent

            onVisibleChanged: {
                if (visible) {
                    statusText.text = "Buffers"
                }
            }

            // Fuck you very much
            Component.onCompleted: {
                var modelIndex = rootIndex
                for (var i=0; i<model.rowCount(); i++) {
                    expand(model.index(i, 0))
                }
            }

            function selectBuffer(index) {
            }

            onActivated: {
                currentBuffer = BufferModel.getBufferId(index)
                stackView.push(messagesComponent)
            }

            TableViewColumn {
                title: "Buffers"
                role: "display"
            }

            style: TreeViewStyle {
                activateItemOnSingleClick: true
                alternateBackgroundColor: backgroundColor
                headerDelegate: Item {}
            }
        }
        }
    }

    Component {
        id: messagesComponent
        MessagesView {
            id: messagesView
            bufferId: currentBuffer
            onBufferNameChanged:  {
                statusText.text = bufferName
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: "<"
                opacity: stackView.depth > 2 ? 1 : 0
                Behavior on opacity { NumberAnimation {} }
                visible: opacity > 0
                enabled: visible
                x: stackView.currentItem ? stackView.currentItem.x : 0
                onClicked: stackView.pop()
            }

            Item { Layout.fillWidth: true }
            Label {
                id: statusText
                width: parent.width - debugButton.width

                Connections {
                    target: BacklogManager
                    enabled: !window.loggedIn
                    onMessagesRequested: statusText.text = message
                    onMessagesProcessed: statusText.text = message
                    onUpdateProgress: statusText.text = "Received messages for " + received + " of " + total + " buffers"
                }
                Connections {
                    target: CoreConnection
                    onConnectionError: statusText.text = errorMsg
                    onProgressTextChanged: statusText.text = msg
                    onConnectionMsg: statusText.text = msg
                }
            }

            Item { Layout.fillWidth: true }
            ToolButton {
                id: debugButton
                text: qsTr('Debug')
                checkable: true
//                onClicked:{
//                    debugWindow.visible = !debugWindow.visible

////                    Qt.openUrlExternally(bugUrl)
//                }
            }
        }
    }

    Rectangle {
        id: debugWindow
        anchors {
            fill: parent
        }
        color: Qt.rgba(1, 1, 1, 0.75)
        visible: debugButton.checked
        Flickable {
            anchors.fill: parent
            contentHeight: logEdit.height
            Text {
                id: logEdit
                wrapMode: Text.Wrap
                width: parent.width

                Connections {
                    target: Client
                    onLogUpdated: {
                        logEdit.text = msg + logEdit.text
                    }
                }
            }
        }
    }
}
