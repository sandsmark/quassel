import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    id: bufferPage
    property string themeNamespace: 'Ubuntu.Components.Themes.'
    header: PageHeader {
        title: qsTr('Quassel IRC')
        extension: HeaderExtensionLabel {
            text: statusbarMessage
        }
        flickable: networksList
        clip: true
        leadingActionBar {
            actions: [
                Action {
                    iconName: 'help'
                    text: qsTr('&Help')
                    onTriggered: Qt.openUrlExternally(bugUrl)
                },
                Action {
                    text: qsTr('Core Accounts')
                    enabled: false
                    visible: accountModel.count > 1
                },
                Action {
                    iconName: 'account'
                    text: accountModel.get(0).accountName
                    visible: accountModel.count > 1
                    onTriggered: accountModel.lastAccountId = accountModel.get(0).accountId
                },
                Action {
                    iconName: 'account'
                    text: accountModel.get(1).accountName
                    visible: accountModel.count > 1
                    onTriggered: accountModel.lastAccountId = accountModel.get(1).accountId
                },
                Action {
                    iconName: 'account'
                    text: accountModel.get(2).accountName
                    visible: accountModel.count > 2
                    onTriggered: accountModel.lastAccountId = accountModel.get(2).accountId
                },
                Action {
                    iconName: 'list-add'
                    text: qsTr('Add Core Account')
                    onTriggered: adaptivePageLayout.addPageToNextColumn(bufferPage, Qt.resolvedUrl('AccountSettings.qml'), {
                        accountModel: accountModel
                    })
                    visible: accountModel.count < 3
                }
            ]
            numberOfSlots: 0
        }
        trailingActionBar.actions: [
            Action {
                id: lightTheme
                iconName: 'torch-on'
                onTriggered: theme.name = themeNamespace + 'Ambiance'
                visible: theme.name.indexOf('SuruDark') > -1
            },
            Action {
                iconName: 'torch-off'
                onTriggered: theme.name = themeNamespace + 'SuruDark'
                visible: !lightTheme.visible
            }
        ]
    }

    AccountModel {
        id: accountModel
    }

    UbuntuListView {
        id: networksList
        width: parent.width
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        clip: true
        model: BufferModel { }
        highlight: null
        delegate: ListItem {
            id: networkItem
            height: networkLabel.height
            expansion.height: networkLabel.height + buffersList.height
            onClicked: expansion.expanded = !expansion.expanded
            Keys.forwardTo: [buffersList]
            Component.onCompleted: {
                networksList.ViewItems.expansionFlags = 0
                expansion.expanded = true
            }
            Column {
                width: parent.width
                ListItemLayout {
                    id: networkLabel
                    title.text: network
                    title.font.weight: Font.Bold
                    width: parent.width
                }

                UbuntuListView {
                    id: buffersList
                    property real labelHeight: units.gu(4)
                    property real spacing: units.gu(2)
                    width: parent.width
                    height: count * (labelHeight + spacing)
                    visible: networkItem.expansion.expanded
                    model: buffers
                    delegate: ListItem {
                        id: bufferItem
                        height: bufferItemLayout.height
                        ListItemLayout {
                            id: bufferItemLayout
                            title.text: modelData.bufferName
                            ShortcutBubble {
                                action: bufferSwitchAction
                                SlotsLayout.position: SlotsLayout.Leading
                                visible: altPressed
                            }
                            ShortcutBubble {
                                visible: modelData.activity
                                color: UbuntuColors.orange
                                SlotsLayout.position: SlotsLayout.Trailing
                            }
                        }
                        action: Action {
                            id: bufferSwitchAction
                            onTriggered: {
                                buffersList.currentIndex = index
                                adaptivePageLayout.addPageToNextColumn(bufferPage, Qt.resolvedUrl('MessagesView.qml'), {
                                    bufferId: modelData.bufferId,
                                    networkId: modelData.networkId
                                })
                            }
                            shortcut: (modelData.bufferId < 10)
                                ? 'Alt+%1'.arg(modelData.bufferId)
                                : 'Alt+%1'.arg(String.fromCharCode(97 + modelData.bufferId - 10))
                        }
                    }
                }
            }
        }
    }
}
