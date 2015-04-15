import QtQuick 2.3

Item {
    id: dropdown
    property alias text: currentText.text
    property bool expanded: false
    property alias dataModel: dropRepeater.model
    property int currentIndex: 0
    height: 30

    onExpandedChanged: if(expanded) window.currentItem = dropdown
    function hide() { expanded = false }

    function containsPoint(px, py) {
        if(px < 0) return false
        if(px > width) return false
        if(py < 0) return false
        if(py > headerRect.y + dropRect.height) return false
        return true
    }

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

            MouseArea {
                id: dropdownArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: dropdown.expanded = !dropdown.expanded
            }
        }
    }

    Rectangle {
        id: dropRect
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: headerRect.bottom
        anchors.topMargin: -3
        radius: 4
        clip: true
        height: dropdown.expanded ? dropColumn.height + 3 : 0
        color: "#2B2B2B"

        Behavior on height {
            NumberAnimation { duration: 100; easing.type: Easing.InQuad }
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 4
            color: "#161616"
        }

        Rectangle {
            anchors.fill: parent
            anchors.bottomMargin: 1
            radius: 4
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
                                dropdown.expanded = false
                                currentText.text = name
                                dropdown.currentIndex = index
                            }
                        }
                    }
                }
            }
        }
    }
}
