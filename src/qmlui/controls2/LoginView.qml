import QtQuick 2.4
import QtQml 2.2
import QtQuick.Controls 2.0
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
            statusLabel.text = "Received messages for " + received + " of " + total + " buffers"
        }
        onMessagesRequested: statusLabel.text = message
        onMessagesProcessed: {
            loggedIn = true
            progressBar.visible = false
        }
    }

    Connections {
        target: AccountModel
        onError: statusLabel.text = "Account error: " + message
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
        onConnectionError: statusLabel.text = errorMsg
        onProgressTextChanged: statusLabel.text = msg
        onConnectionMsg: statusLabel.text = msg

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
            statusLabel.text = "Error: Invalid username or password"
        }
        onHandleNoSslInClient: {
            statusLabel.text = "Error: Your client does not support encryption"
        }
        onHandleNoSslInCore: {
            statusLabel.text = "Error: The core you tried to connect to does not support encryption"
        }
        onHandleSslErrors: {
            statusLabel.text = "Error: Encryption error during connection"
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

        GridLayout {
            id: loginGrid
            columns: 2
            rowSpacing: 10
            Layout.alignment: Qt.AlignHCenter
            
            enabled: CoreConnection.state === CoreConnection.Disconnected
            
            Label {
                id: statusLabel
                visible: text != ""
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                Layout.columnSpan: 2
                Connections {
                    target: CoreConnection
                }
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
            ComboBox {
                id: accountSelect
                model: AccountModel
                textRole: "accountName"
                currentIndex: AccountModel.indexOfLastAccount()
                Layout.columnSpan: 2
                Layout.fillWidth: true
                visible: !editMode
                onCurrentIndexChanged:{
                    currentAccount = AccountModel.get(currentIndex)
                    changed = false
                    if (currentAccount.accountName === "") {
                        editMode = true
                    }
                }
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
                visible: !editMode
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
            visible: !editMode
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
        Item {
            Layout.fillHeight: true
            visible: editMode
        }
    }
}
