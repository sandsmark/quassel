import QtQuick 2.4
import QtQml 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import Quassel 0.1

Item {
    id: loginView
    
    property bool editMode: (currentAccount === -1)
    property bool changed: false
    property var currentAccount: AccountModel.get(AccountModel.indexOfLastAccount())

    property bool loggedIn: false

    Connections {
        target: BacklogManager

        // Progress handling
        onUpdateProgress: {
            if (!total) {
                return
            }
            progressBar.indeterminate = false
            progressBar.value = received / total
        }
        onMessagesProcessed: {
            loggedIn = true
            progressBar.visible = false
        }
    }

    Connections {
        target: CoreConnection
        property int max: 10
        property int min: 0

        onStateChanged: {
            if (newState === CoreConnection.Disconnected) {
                loginView.loggedIn = false
            }
        }
        onConnectionError: errorLabel.text = errorMsg

        // Connection progress handling
        onProgressRangeChanged: {
            max = maximum
            min = minimum
        }
        onProgressValueChanged: {
            if (max - min === 0) {
                return
            }
            if (value === max) {
                value = 0 // prepare for next
                progressBar.indeterminate = true
            } else {
                progressBar.indeterminate = false
                progressBar.value = (value - min) / (max - min)
            }
        }

        // Error handling
        onUserAuthenticationRequired: {
            errorLabel.text = "Error: Invalid username or password"
        }
        onHandleNoSslInClient: {
            errorLabel.text = "Error: Your client does not support encryption"
        }
        onHandleNoSslInCore: {
            errorLabel.text = "Error: The core you tried to connect to does not support encryption"
        }
        onHandleSslErrors: {
            errorLabel.text = "Error: Encryption error during connection"
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        Item {
            Layout.fillHeight: true
            visible: !editMode
        }
        
        Label {
            id: errorLabel
            visible: text != ""
            wrapMode: Text.Wrap
            Layout.maximumWidth: accountSelect.width
            Layout.minimumWidth: accountSelect.width
            Layout.alignment: Qt.AlignHCenter
            Connections {
                target: CoreConnection
            }
        }
        
        GridLayout {
            id: loginGrid
            columns: 2
            rowSpacing: 10
            //                    Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            //                    Layout.alignment: Qt.AlignCenter
            
            enabled: CoreConnection.state === CoreConnection.Disconnected
            
            
            ComboBox {
                id: accountSelect
                model: AccountModel
                textRole: "accountName"
                currentIndex: AccountModel.lastAccountId
                Layout.columnSpan: 2
                Layout.fillWidth: true
                onCurrentIndexChanged:{
                    currentAccount = AccountModel.get(currentIndex)
                    changed = false
                }
                activeFocusOnPress: true
            }
            Label {
                text: "Account name:"
                visible: editMode
            }
            TextField {
                id: accountNameField
                text: currentAccount.accountName
                visible: editMode
                onTextChanged: {
                    currentAccount.accountName = text
                    changed = true
                }
            }

            Item {
                height: 10
                Layout.columnSpan: 2
            }
            
            Label {
                text: "Username:"
            }
            TextField {
                id: usernameField
                text: currentAccount.user
                onFocusChanged: {
                    if (focus) {
                        editMode = true
                    }
                }
                onTextChanged: {
                    currentAccount.username = text
                    changed = true
                }
            }
            
            Label {
                text: "Password:"
                visible: editMode
            }
            TextField {
                id: passwordField
                text: currentAccount.password
                echoMode: TextInput.Password
                visible: editMode
                onTextChanged: {
                    currentAccount.password = text
                    changed = true
                }
            }
            Label {
                text: "Hostname:"
                visible: editMode
            }
            TextField {
                id: hostnameField
                text: currentAccount.hostname
                visible: editMode
                onTextChanged: {
                    currentAccount.hostname = text
                    changed = true
                }
            }
            Label {
                text: "Port:"
                visible: editMode
            }
            TextField {
                id: portField
                text: currentAccount.port
                visible: editMode
                onTextChanged: {
                    currentAccount.port = text
                    changed = true
                }
            }
            Component.onCompleted: changed = false
            
            
            Item {
                height: 20
                Layout.columnSpan: 2
            }

            Button {
                Layout.columnSpan: 2
                Layout.fillWidth: true
                visible: editMode
                text: "Delete account"
                onClicked: {
                    AccountModel.removeAccount(currentAccount.accountId)
                    currentAccount = AccountModel.get(accountSelect.currentIndex)
                }
            }
            
            ProgressBar {
                id: progressBar
                value: 0
                indeterminate: true
                visible: CoreConnection.state !== CoreConnection.Disconnected
                Layout.columnSpan: 2
                Layout.fillWidth: true

            }
        }

        Item {
            Layout.fillHeight: true
        }
        Button {
            text: changed ? qsTr('Save and connect') : qsTr('Connect')
            onClicked: {
                if (changed) {
                    editMode = false
                    if (!AccountModel.createOrUpdateAccount({
                                                           AccountId: currentAccount.accountId,
                                                           AccountName: accountNameField.text,
                                                           HostName: hostnameField.text,
                                                           Port: portField.text,
                                                           User: usernameField.text,
                                                           Password: passwordField.text,
                                                       })) {
                        errorLabel.text = "Unable to save account, missing info"
                        return;
                    }

                    changed = false
                } else {
                    CoreConnection.connectToCore(currentAccount.accountId)
                }
            }
            enabled: CoreConnection.state === CoreConnection.Disconnected
            Layout.fillWidth: true
        }
    }
}
