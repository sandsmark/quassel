import QtQuick 2.4
import QtQml 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import Quassel 0.1

ApplicationWindow {
    id: window
    width: 1920
    height: 1080
    visible: true
    flags: Qt.Dialog

    title: qsTr('Quassel IRC')

    AccountModel {
        id: accountModel
    }

    AccountSettings {
        id: accountSettingsDialog
        accountModel: accountModel
    }

    Item {
        anchors.fill: parent
        clip: true

        Label {
            id: content

            text: "Quassel"
            font.pixelSize: 96
            anchors.fill: parent
            verticalAlignment: Label.AlignVCenter
            horizontalAlignment: Label.AlignHCenter
            opacity: 1 - bufferView.opacity
        }

        ButtonGroup {
            id: bufferGroup
        }

        Frame {
            id: bufferView
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: progressBar.value > 0.99 ? parent.width / 5 : 0
        ListView {
            anchors.fill:parent
            opacity: progressBar.value > 0.99 ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 300 } }
            Behavior on width { NumberAnimation { duration: 100 } }
            model: BufferModel

            delegate: Column {
                width: parent.width
                RadioDelegate {
                    id: networkHeader
                    text: network
                    font.bold: true
                    width: parent.width
                    ButtonGroup.group: bufferGroup
                }
                id: channelList
                Repeater {
                    model: buffers
                    property int elementHeight: 20
                    height: count * networkHeader.height
                    delegate: RadioDelegate {
                        x: 20
                        text: modelData.bufferName
                        width: channelList.width - x
                        onClicked: messagesView.bufferId = modelData.bufferId
                    }
                }
            }
            ScrollIndicator.vertical: ScrollIndicator { }
        }
        }

//        TreeView {
//            id: networksList
//            width: parent.width / 4
//            Layout.minimumWidth: 400
//            model: BufferModel

//            function selectBuffer(index) {
//                messagesView.bufferId = BufferModel.getBufferId(index)
//            }

//            onActivated: selectBuffer(index)

//            TableViewColumn {
//                title: "Buffers"
//                role: "display"
//            }

//            style: TreeViewStyle {
//                activateItemOnSingleClick: true
//                alternateBackgroundColor: backgroundColor
//            }
//        }

        MessagesView {
            id: messagesView
            opacity: bufferView.opacity
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
                left: bufferView.right
            }

            Layout.fillWidth: true
        }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr('Connect')
                onClicked: CoreConnection.connectToCore(accountModel.get(accountSelect.currentIndex).accountId)
                visible: CoreConnection.state === CoreConnection.Disconnected
            }
            ComboBox {
                id: accountSelect
                model: accountModel
                visible: CoreConnection.state === CoreConnection.Disconnected
                textRole: "accountName"
                Component.onCompleted: {
                    currentIndex = accountModel.indexOfLastAccount()
                }
            }

            Item { Layout.fillWidth: true }
            ToolButton {
                text: qsTr('Help')
                onClicked: Qt.openUrlExternally(bugUrl)
            }
        }
    }

    footer: Frame {
        RowLayout {
            anchors.fill: parent
            Label {
                id: statusText

                Connections {
                    target: CoreConnection
                    onConnectionError: statusText.text = errorMsg
                    onProgressTextChanged: statusText.text = msg
                    onConnectionMsg: statusText.text = msg
                }
            }
            Item { Layout.fillWidth: true }
            ProgressBar {
                id: progressBar
                value: 0
                indeterminate: value < 0.2
                visible: CoreConnection.state === CoreConnection.Synchronized && value < 0.99

                Connections {
                    target: BacklogManager
                    onUpdateProgress: {
                        if (!total) {
                            return
                        }
                        progressBar.value = received / total
                        if (progressBar.value > 0.9) {
                        }
                    }
                }
            }
        }
    }
}
