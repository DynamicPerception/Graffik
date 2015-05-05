import QtQuick 2.3
import Graffik.Components 1.0

Item {
    id: dropdown
    property alias text: currentText.text
    property alias dataModel: dropRepeater.model
    property int currentIndex: 0
    height: 30

    Rectangle {
        id: headerRect
        anchors.fill: parent
        radius: 4
        color: "#2B2B2B"

        Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: 1
            radius: 4
            color: "#161616"

            Text {
                id: currentText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: buttonImage.left
                anchors.leftMargin: 5
                anchors.rightMargin: 5

                font.family: fontLight.name
                font.pixelSize: 18
                renderType: Text.NativeRendering
                elide: Text.ElideRight
                color: "#AAAAAA"
            }

            Image {
                id: buttonImage
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 2
                source: "qrc:///images/ui/dropButton.png"

                Image {
                    anchors.centerIn: parent
                    rotation: popup.visible ? 180 : 0
                    source: {
                        if(dropdownArea.pressed)
                            return "qrc:///images/ui/arrowPressed.png"
                        if(dropdownArea.containsMouse)
                            return "qrc:///images/ui/arrowHovered.png"
                        return "qrc:///images/ui/arrow.png"
                    }
                }
            }

            MouseArea {
                id: dropdownArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: popup.show()
            }
        }
    }

    PopupWindow {
        id: popup
        objectName: "popupWindow"
        height: dropRect.height
        width: dropdown.width
        parentItem: dropdown
        yOffset: 26
        color: "transparent"

        Rectangle {
            id: dropRect
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            radius: 4
            clip: true
            height: dropColumn.height + 3
            color: "#161616"

            Column {
                id: dropColumn
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: 3

                Repeater {
                    id: dropRepeater
                    onCountChanged: if(count === 0) currentText.text = ""
                    delegate: Item {
                        width: dropColumn.width
                        height: 30

                        Component.onCompleted: if(index === 0) currentText.text = name
                        Connections {
                            target: dropdown
                            onCurrentIndexChanged: if(dropdown.currentIndex === index) currentText.text = name
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            font.family: fontLight.name
                            font.pixelSize: 18
                            renderType: Text.NativeRendering
                            elide: Text.ElideRight
                            color: "#AAAAAA"
                            text: name
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                currentText.text = name
                                dropdown.currentIndex = index
                                popup.hide()
                            }
                        }
                    }
                }
            }
        }
    }
}
