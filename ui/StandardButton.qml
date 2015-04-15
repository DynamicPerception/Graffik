import QtQuick 2.3

BorderImage {
    id: button
    property alias text: label.text
    property alias image: icon.source
    property alias fontSize: label.font.pixelSize
    signal clicked()

    source: {
        if(buttonArea.pressed) return "qrc:///images/ui/standardButtonBgPressed.png"
        if(buttonArea.containsMouse) return "qrc:///images/ui/standardButtonBgHovered.png"
        return "qrc:///images/ui/standardButtonBg.png"
    }

    border.left: 2
    border.right: 2
    border.top: 2
    border.bottom: 2
    height: 30

    Text {
        id: label
        anchors.centerIn: parent
        font.family: "Arial"
        font.pixelSize: 16
        renderType: Text.NativeRendering
        color: "#FFFFFF"
    }

    Image {
        id: icon
        anchors.centerIn: parent
    }

    MouseArea {
        id: buttonArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: button.clicked()
    }
}
