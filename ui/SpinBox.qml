import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0

Item {
    id: spinBox
    property alias validator: input.validator
    property alias text: input.text
    signal increaseClicked()
    signal decreaseClicked()
    width: 80; height: 30

    Rectangle {
        width: parent.width
        height: 29
        x: 0; y: 1
        radius: 4
        color: "#2B2B2B"
    }

    Rectangle {
        width: parent.width
        height: 29
        x: 0; y: 0
        radius: 4
        color: "#161616"

        TextField {
            id: input
            anchors.fill: parent
            anchors.rightMargin: 22
            verticalAlignment: Text.AlignVCenter
            readOnly: true

            style: TextFieldStyle {
                textColor: "#AAAAAA"
                font.family: fontLight.name
                font.pixelSize: 18
                renderType: Text.NativeRendering
                background: Rectangle { color: "transparent" }
            }
        }

        Rectangle {
            id: buttons
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 2
            anchors.right: parent.right
            width: 22
            y: 0; radius: 4
            color: "#2B2B2B"

            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: 1
                color: "#242424"
            }

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 1
                width: 22; height: 1
                color: "#2B2B2B"
            }

            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: 22; height: 1
                color: "#161616"
            }

            Rectangle {
                width: 4; height: 27
                x: 0; y: 0
                color: "#161616"
            }

            Column {
                anchors.fill: parent
                anchors.leftMargin: 4

                Item {
                    width: parent.width
                    height: parent.height / 2

                    Image {
                        id: upImage
                        anchors.centerIn: parent
                        rotation: 180
                        source: {
                            if(upArea.pressed)
                                return "qrc:///images/ui/arrowPressed.png"
                            if(upArea.containsMouse)
                                return "qrc:///images/ui/arrowHovered.png"
                            return "qrc:///images/ui/arrow.png"
                        }
                    }

                    RectangularGlow {
                        anchors.fill: upImage
                        visible: upArea.containsMouse && !upArea.pressed
                        glowRadius: 2; spread: 0
                        color: "#67ADF7"
                        cornerRadius: 4
                    }

                    MouseArea {
                        id: upArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: spinBox.increaseClicked()
                    }
                }

                Item {
                    width: parent.width
                    height: parent.height / 2

                    Image {
                        id: downImage
                        anchors.centerIn: parent
                        source: {
                            if(downArea.pressed)
                                return "qrc:///images/ui/arrowPressed.png"
                            if(downArea.containsMouse)
                                return "qrc:///images/ui/arrowHovered.png"
                            return "qrc:///images/ui/arrow.png"
                        }
                    }

                    RectangularGlow {
                        anchors.fill: downImage
                        visible: downArea.containsMouse && !downArea.pressed
                        glowRadius: 2; spread: 0
                        color: "#67ADF7"
                        cornerRadius: 4
                    }

                    MouseArea {
                        id: downArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: spinBox.decreaseClicked()
                    }
                }
            }
        }
    }
}
