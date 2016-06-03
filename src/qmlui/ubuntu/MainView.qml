import QtQuick 2.4
import Ubuntu.Components 1.3

MainView {
    applicationName: 'quassel.kalikiana'
    width: units.gu(100)
    height: units.gu(75)

    Binding {
        target: typeof window !== 'undefined' ? window : null
        property: 'title'
        value: qsTr('Quassel IRC')
    }

    AdaptivePageLayout {
        id: adaptivePageLayout
        anchors.fill: parent
        primaryPageSource: Qt.resolvedUrl('BufferView.qml')
        asynchronous: false
    }
}
