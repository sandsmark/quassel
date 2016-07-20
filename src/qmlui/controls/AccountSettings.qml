import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import Quassel 0.1

Dialog {
    property AccountModel accountModel
    property int accountId

    title: qsTr("Account settings")
    standardButtons: StandardButton.Save | StandardButton.Cancel

    id: settingsPage

    onAccepted: {
//                    accountModel.createOrUpdateAccount({
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
            text: accountModel.get(accountId).accountName
        }

        Label {
            text: qsTr('Hostname:')
        }
        TextField {
            id: hostnameInput
            text: accountModel.get(accountId).hostname
        }

        Label {
            text: qsTr('Port:')
        }
        TextField {
            id: portInput
            text: accountModel.get(accountId).port
            placeholderText: "4242"
        }

        Label {
            text: qsTr('User:')
        }

        TextField {
            id: userInput
            text: accountModel.get(accountId).user
        }

        Label {
            text: qsTr('Password:')
        }

        TextField {
            id: passwordInput
            echoMode: TextInput.Password
            text: accountModel.get(accountId).password
        }

        Button {
            text: qsTr("Delete account")
//            onClicked: {
//                accountModel.removeAccount(accountId)
//        }
        }
    }
}
