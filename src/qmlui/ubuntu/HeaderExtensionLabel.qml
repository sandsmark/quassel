import QtQuick 2.4
import Ubuntu.Components 1.3

StyledLabel {
    anchors {
        left: parent.left
        right: parent.right
        leftMargin: units.gu(2)
        verticalCenter: parent.verticalCenter
        verticalCenterOffset: -units.gu(1)
    }
    color: theme.palette.disabled.backgroundText
    // Click/ tap to expand to multiple lines
    wrapMode: activeFocus ? Text.Wrap : Text.NoWrap

    MouseArea {
        anchors.fill: parent
        enabled: !parent.activeFocus
        onClicked: parent.forceActiveFocus()
        Mouse.forwardTo: [parent]
    }
}
