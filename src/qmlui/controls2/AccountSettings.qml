import QtQuick 2.4
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Quassel 0.1

Dialog {
    property int accountId

    title: qsTr("Account settings")
    standardButtons: StandardButton.Save | StandardButton.Cancel

    id: settingsPage

    onAccepted: {
//                    AccountModel.createOrUpdateAccount({
//                                                           AccountId: accountId,
//                                                           AccountName: accountNameInput.value,
//                                                           HostName: hostnameInput.value,
//                                                           Port: portInput.value,
//                                                           User: userInput.value,
//                                                           Password: passwordInput.value,
//                                                       })
    }

    GridLayout {
        rowSpacing: 2

        columns: 2
        Layout.alignment: Qt.AlignHCenter

        Label {
            text: qsTr('Account Name:')
        }
        TextField {
            id: accountNameInput
            text: AccountModel.get(accountId).accountName
        }

        Label {
            text: qsTr('Hostname:')
        }
        TextField {
            id: hostnameInput
            text: AccountModel.get(accountId).hostname
        }

        Label {
            text: qsTr('Port:')
        }
        TextField {
            id: portInput
            text: AccountModel.get(accountId).port
            placeholderText: "4242"
        }

        Label {
            text: qsTr('User:')
        }

        TextField {
            id: userInput
            text: AccountModel.get(accountId).user
        }

        Label {
            text: qsTr('Password:')
        }

        TextField {
            id: passwordInput
            echoMode: TextInput.Password
            text: AccountModel.get(accountId).password
        }

        Button {
            text: qsTr("Delete account")
//            onClicked: {
//                AccountModel.removeAccount(accountId)
//        }
        }
    }
}
