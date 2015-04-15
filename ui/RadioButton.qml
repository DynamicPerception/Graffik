import QtQuick 2.3

Item {
    id: button
    property bool checked: false
    property alias text: label.text
    signal clicked()
    height: 22

    Row {
        anchors.fill: parent
        spacing: 10

        Image {
            anchors.verticalCenter: parent.verticalCenter
            source: button.checked ? "qrc:///images/ui/radialOn.png" :
                                     "qrc:///images/ui/radialOff.png"
        }

        Text {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            font.family: fontRoman.name
            font.pixelSize: 13
            renderType: Text.NativeRendering
            color: "#818181"
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            button.checked = true
            button.clicked()
        }
    }
}
