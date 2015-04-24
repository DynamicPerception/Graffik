import QtQuick 2.3
import Graffik.Components 1.0
import QtQuick.Window 2.2

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

            MouseArea {
                id: dropdownArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    popup.position = dropdown.mapToItem(rootItem, 0, dropdown.height - 2)
                    popup.show()
                }
            }

            Text {
                id: currentText
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: rescanRect.left
                anchors.leftMargin: 5
                anchors.rightMargin: 5

                font.family: fontLight.name
                font.pixelSize: 18
                renderType: Text.NativeRendering
                elide: Text.ElideRight
                color: "#AAAAAA"
            }

            Rectangle {
                id: rescanRect
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: buttonImage.left
                anchors.rightMargin: 1
                anchors.topMargin: 2
                anchors.bottomMargin: 2
                width: 26
                color: rescanArea.pressed ? "#2B2B2B" : "#363636"

                Image {
                    anchors.centerIn: parent
                    source: "qrc:///images/ui/rescan.png"
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 1
                    color: "#2B2B2B"
                }

                MouseArea {
                    id: rescanArea
                    anchors.fill: parent
                    onClicked: window.portsRescanClicked()
                }
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
                    rotation: dropdown.expanded ? 180 : 0
                    source: {
                        if(dropdownArea.pressed)
                            return "qrc:///images/ui/arrowPressed.png"
                        if(dropdownArea.containsMouse)
                            return "qrc:///images/ui/arrowHovered.png"
                        return "qrc:///images/ui/arrow.png"
                    }
                }
            }
        }
    }

    PopupWindow {
        id: popup
        height: dropRect.height
        property var position: dropdown.mapToItem(rootItem, 0, dropdown.height - 2)
        x: position.x + window.x
        y: position.y + window.y
        width: dropdown.width
        color: "transparent"

        Rectangle {
            id: dropRect
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            //radius: 4
            clip: true
            height: dropColumn.height + 3
            color: "#161616"

            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: 1
                //radius: 4
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
}
