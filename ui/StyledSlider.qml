import QtQuick 2.3

Item {
    id: slider
    property alias title: titleText.text
    property alias from: fromText.text
    property alias to: toText.text
    signal moved(real position)
    height: fromText.y + fromText.height

    Label {
        id: titleText
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 16
    }

    Rectangle {
        id: body
        anchors.top: titleText.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 5
        height: 31
        radius: 4
        color: "#2B2B2B"

        Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: 1
            radius: 4
            color: "#161616"

            Image {
                id: handler
                property int minimumX: 2
                property int maximumX: body.width - 2 - width
                anchors.verticalCenter: parent.verticalCenter
                x: maximumX
                source: mouseArea.containsMouse ? "qrc:///images/ui/styledSliderHandlerHovered.png" :
                                                  "qrc:///images/ui/styledSliderHandler.png"

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    drag.target: handler
                    drag.axis: Drag.XAxis
                    drag.minimumX: parent.minimumX
                    drag.maximumX: parent.maximumX
                    onReleased: slider.moved((parent.x - parent.minimumX) / parent.maximumX)
                }
            }
        }
    }

    Label {
        id: fromText
        anchors.left: parent.left
        anchors.top: body.bottom
        anchors.topMargin: 5
    }

    Label {
        id: toText
        anchors.right: parent.right
        anchors.top: body.bottom
        anchors.topMargin: 5
    }
}
