import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    id: messagesPage
    property alias bufferId: messagesModel.bufferId
    property alias networkId: messagesModel.networkId
    property alias nick: nickLabel.text

    header: PageHeader {
        id: messagesHeader
        title: messagesModel.bufferName
        subtitle: messagesModel.topic
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
        focus: true
        clip: true
        highlight: null
        model: MessageModel {
            id: messagesModel
        }
        delegate: ListItem {
            height: messageItemLayout.height
            divider.visible: model.divider || lastSeen
            divider.colorFrom: lastSeen ? UbuntuColors.orange : theme.palette.normal.base
            property bool lastSeen: messagesModel.lastSeenMsgId == model.msgId
            ListItemLayout {
                id: messageItemLayout
                subtitle.text: model.divider ? '<b>%2</b> %1'.arg(i18n.relativeDateTime(timestamp)).arg(sender.split('!')[0]) : ''
                title.text: message.replace('>', '&gt;').replace('<', '&lt;').replace('&', '&amp;').replace(/(https?:\/\/[^\s]+)/g, '<a href="$1">$1</a>')
                title.textFormat: Text.StyledText
                title.linkColor: UbuntuColors.orange
                title.onLinkActivated: Qt.openUrlExternally(link)
                title.maximumLineCount: 0
            }
        }
    }

    TextField {
        id: messageInput
        anchors {
            bottom: messagesPage.bottom
            topMargin: units.gu(0.5)
            bottomMargin: units.gu(0.5)
        }
        width: parent.width
        enabled: false // FIXME: messagesModel.connected
        primaryItem: Label {
            id: nickLabel
            text: messagesModel.nick
            enabled: false
        }
    }
}
