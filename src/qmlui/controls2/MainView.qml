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
    visibility: "Maximized"

    title: qsTr('Quassel IRC')

    property bool loggedIn: false
    onLoggedInChanged: {
        if (loggedIn) {
            stackView.push(mainComponent)
        } else {
            stackView.clear()
            stackView.push(loginComponent)
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
        anchors.fill: parent
        initialItem: loginComponent
    }

    Component {
        id: mainComponent
        Item {
            Drawer {
                id: bufferDrawer
                width: parent.width * 0.75
                height: parent.height

                ButtonGroup {
                    id: bufferGroup
                }
                ListView {
                    anchors.fill:parent
                    model: BufferModel

                    delegate: Column {
                        width: parent.width
                        ItemDelegate {
                            id: networkHeader
                            text: network
                            font.bold: true
                            width: parent.width
                            ButtonGroup.group: bufferGroup
                        }
                        id: channelList
                        Repeater {
                            model: buffers
                            height: count * networkHeader.height
                            delegate: ItemDelegate {
                                x: 20
                                text: modelData.bufferName
                                width: channelList.width - x
                                onClicked: {
                                    messagesView.bufferId = modelData.bufferId
                                    bufferDrawer.close()
                                }
                            }
                        }
                    }
                    ScrollIndicator.vertical: ScrollIndicator { }
                }
            }

            MessagesView {
                id: messagesView
                opacity: loggedIn ? 1 : 0
                anchors.fill: parent

                transform: Translate {
                    x: bufferDrawer.position * messagesView.width / 2
                }
            }
            Component.onCompleted: bufferDrawer.open()
        }
    }
}
