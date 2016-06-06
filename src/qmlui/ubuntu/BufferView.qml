import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    id: bufferPage
    property string themeNamespace: 'Ubuntu.Components.Themes.'
    header: PageHeader {
        title: qsTr('Quassel IRC')
        subtitle: statusbarMessage
        flickable: networksList
        clip: true
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

    UbuntuListView {
        id: networksList
        anchors.fill: parent
        focus: true
        clip: true
        model: BufferModel { }
        highlight: null
        delegate: ListItem {
            id: networkItem
            height: networkLabel.height
            expansion.height: networkLabel.height + buffersList.height
            onClicked: expansion.expanded = !expansion.expanded
            Component.onCompleted: {
                networksList.ViewItems.expansionFlags = 0
                expansion.expanded = true
            }
            Column {
                anchors.fill: parent
                Label {
                    id: networkLabel
                    text: network
                    height: buffersList.labelHeight
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
                        height: bufferItemLayout.height
                        ListItemLayout {
                            id: bufferItemLayout
                            title.text: modelData.bufferName
                            UbuntuShape {
                                width: units.gu(4)
                                height: width
                                visible: modelData.activity
                                Label {
                                    anchors.centerIn: parent
                                    text: modelData.activity
                                }
                                SlotsLayout.position: SlotsLayout.Trailing
                            }
                        }
                        onClicked: {
                            buffersList.currentIndex = index
                            adaptivePageLayout.addPageToNextColumn(bufferPage, Qt.resolvedUrl('MessagesView.qml'), {
                                bufferId: modelData.bufferId,
                                networkId: modelData.networkId
                            })
                        }
                    }
                }
            }
        }
    }
}
