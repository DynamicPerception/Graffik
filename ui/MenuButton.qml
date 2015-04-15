import QtQuick 2.3

Item {
    id: button
    property alias text: label.text
    property string checkedImage
    property string uncheckedImage
    signal clicked()
    width: 60; height: 60

    Image {
        id: image
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: label.top
        anchors.bottomMargin: 5
        source: buttonArea.containsMouse ? button.checkedImage :
                                           button.uncheckedImage
    }

    Text {
        id: label
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        font.family: fontRoman.name
        font.pixelSize: 13
        renderType: Text.NativeRendering
        color: buttonArea.containsMouse ? "#FFFFFF" : "#464646"
    }

    MouseArea {
        id: buttonArea
        anchors.fill: parent
        enabled: button.enabled
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        onClicked: parent.clicked()
    }
}
