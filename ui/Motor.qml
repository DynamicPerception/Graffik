import QtQuick 2.3
import QtQuick.Controls 1.2
import "qrc:///components/ui" as Components

Components.Box {
    id: box

    Button {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        width: 200
        text: qsTr("switch easing")
        onClicked: {
            plot.easing = (++plot.easing) % 3 + 1
        }
    }

    Components.Plot {
        id: plot
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
    }
}
