import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    id: messagesPage
    property alias bufferName: messagesHeader.title
    property alias topic: messagesHeader.subtitle
    property alias bufferId: messagesModel.bufferId
    property int lastSeenMsgId

    header: PageHeader {
        id: messagesHeader
        flickable: messagesList
        clip: true
    }

    UbuntuListView {
        id: messagesList
        anchors.fill: parent
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
            property bool lastSeen: messagesPage.lastSeenMsgId == model.msgId
            ListItemLayout {
                id: messageItemLayout
                subtitle.text: model.divider ? '<b>%2</b> %1'.arg(i18n.relativeDateTime(timestamp)).arg(sender.split('!')[0]) : ''
                title.text: message
                title.maximumLineCount: 0
            }
        }
    }
}
