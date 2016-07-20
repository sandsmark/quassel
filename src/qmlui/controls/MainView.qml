import QtQuick 2.4
import QtQml 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import Quassel 0.1

ApplicationWindow {
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


    menuBar: MenuBar {
        Menu {
            id: accountsMenu
            title: qsTr("Select account")
            visible: !BufferModel.connected

            Instantiator {
                model: accountModel
                onObjectAdded: accountsMenu.insertItem( index, object )
                onObjectRemoved: accountsMenu.removeItem( object )
                delegate: MenuItem {
                    onTriggered: accountModel.lastAccountId = accountId
                    text: accountName
                }
            }
            MenuItem {
                text: qsTr("Add account...")
                onTriggered: accountSettingsDialog.open()
            }
        }
    }

    SplitView {
        anchors.fill: parent

        TreeView {
            id: networksList
            width: parent.width / 4
            Layout.minimumWidth: 400
            model: BufferModel

            function selectBuffer(index) {
                messagesView.bufferId = BufferModel.getBufferId(index)
            }

            onActivated: selectBuffer(index)

            TableViewColumn {
                title: "Buffers"
                role: "display"
            }

            style: TreeViewStyle {
                activateItemOnSingleClick: true
                alternateBackgroundColor: backgroundColor
            }
        }

        MessagesView {
            id: messagesView
            width: 400
            Layout.fillWidth: true
        }
    }

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: qsTr('Connect')
                onClicked: CoreConnection.connectToCore()
                visible: !BufferModel.connected
            }
            ToolButton {
                text: qsTr('Help')
                onClicked: Qt.openUrlExternally(bugUrl)
            }
            Item { Layout.fillWidth: true }
        }
    }
    statusBar: StatusBar {
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
                visible: value > 0.01 && value < 0.99

                Connections {
                    target: BacklogManager
                    onUpdateProgress: {
                        if (!total) {
                            return
                        }
                        progressBar.value = received / total
                    }
                }
            }
        }
    }

}
