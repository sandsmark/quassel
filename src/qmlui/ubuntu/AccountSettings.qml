import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    property AccountModel accountModel
    property int accountId

    id: settingsPage
    header: PageHeader {
        id: settingsHeader
        title: qsTr('Settings')
        clip: true
        leadingActionBar.actions: [
            Action {
                iconName: 'close'
                onTriggered: adaptivePageLayout.removePages(settingsPage)
            }
        ]
        trailingActionBar.actions: [
            Action {
                iconName: 'ok'
                text: qsTr('Save changes')
                enabled: hostnameInput.value && userInput.value && passwordInput.value
                onTriggered: {
                    accountModel.createOrUpdateAccount({
                        AccountId: accountId,
                        AccountName: accountNameInput.value,
                        HostName: hostnameInput.value,
                        Port: portInput.value,
                        User: userInput.value,
                        Password: passwordInput.value,
                    })
                    adaptivePageLayout.removePages(settingsPage)
                }
            }
        ]
    }

    Column {
        spacing: units.gu(2)

        width: parent.width
        anchors {
            top: settingsHeader.bottom
            topMargin: spacing
            bottom: parent.bottom
        }
        clip: true
        ValueInput {
            id: accountNameInput
            text: qsTr('Account Name:')
            defaultValue: hostnameInput.value
        }
        ValueInput {
            id: hostnameInput
            text: qsTr('Hostname:')
        }
        ValueInput {
            id: portInput
            text: qsTr('Port:')
            defaultValue: 4242
        }
        ValueInput {
            id: userInput
            text: qsTr('User:')
        }
        ValueInput {
            id: passwordInput
            text: qsTr('Password:')
            echoMode: TextInput.Password
        }
    }
}
