import QtQuick 2.4
import Ubuntu.Components 1.3

Column {
    property string label
    property alias text: textField.text
    property var defaultValue: ''
    property string valueType: typeof defaultValue
    property alias echoMode: textField.echoMode
    readonly property var value: {
        if (valueType == 'boolean')
            return option.checked
        if (echoMode == TextInput.Password)
            return textField.text ? textField.text : defaultValue
        return textField.displayText ? textField.displayText : defaultValue
    }

    x: spacing
    width: parent.width - spacing * 2
    spacing: units.gu(1)

    Label {
        text: label.replace(':', '')
    }
    Switch {
        id: option
        visible: valueType == 'boolean'
        checked: defaultValue
    }
    TextField {
        id: textField
        visible: valueType != 'boolean'
        width: parent.width
        placeholderText: String(defaultValue)
        validator: valueType == 'number' ? intValidator : null
        inputMethodHints: valueType == 'string' ? Qt.ImhNoPredictiveText : Qt.ImhDigitsOnly
        IntValidator {
            id: intValidator
            bottom: 0
            top: 9999
        }
    }
}
