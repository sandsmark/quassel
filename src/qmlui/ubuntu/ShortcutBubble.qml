import Ubuntu.Components 1.3

UbuntuShape {
    property string shortcut: action ? action.shortcut : ''
    property Action action

    width: units.gu(2)
    height: width
    color: theme.palette.normal.base
    Label {
        id: label
        anchors.centerIn: parent
        color: theme.palette.normal.baseText
        text: shortcut.replace(/(Alt|Ctrl)\+/, '')
    }
}
