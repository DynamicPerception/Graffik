import QtQuick 2.3
import QtQuick.Controls 1.2
import "qrc:///components/ui" as Components

Components.Box {
    id: box
    property int port: 0
    property bool expanded: true
    height: expanded ? 330 : plot.y + plot.height + 10
    onVisibleChanged: if(visible) plot.repaint()

    Behavior on height {
        NumberAnimation { duration: 100; easing.type: Easing.InQuad }
    }

    Item {
        anchors.top: parent.top
        anchors.right: parent.right
        width: 55; height: 55

        Image {
            anchors.centerIn: parent
            source: "qrc:///images/ui/expand.png"
            rotation: box.expanded ? 0 : 180
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: box.expanded = !box.expanded
        }
    }

    Row {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 20
        anchors.topMargin: 20
        spacing: 10

        Text {
            id: portLabel
            font.family: fontRoman.name
            font.pixelSize: 16
            renderType: Text.NativeRendering
            color: "#FFFFFF"
            text: qsTr("Port")
        }

        Text {
            anchors.bottom: portLabel.bottom
            font.family: fontRoman.name
            font.pixelSize: 21
            renderType: Text.NativeRendering
            color: "#FFFFFF"
            text: box.port
        }
    }

    Components.Plot {
        id: plot
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 55
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: 183
    }

    Item {
        anchors.top: plot.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        clip: height < 50

        Row {
            id: row
            anchors.fill: parent
            spacing: 20
            z: 1

            ListModel {
                id: easingModel
                ListElement { name: "Quadratic" }
                ListElement { name: "Inv. Quadratic" }
                ListElement { name: "Linear" }
            }

            Column {
                width: 130
                spacing: 10

                Components.Dropdown {
                    id: easingTypeDropdown
                    anchors.left: parent.left
                    anchors.right: parent.right
                    width: 100
                    text: "Linear"
                    z: 1
                    dataModel: easingModel
                    onCurrentIndexChanged: {
                        var easing
                        if(currentIndex == 2)
                            easing = 1
                        else
                            easing = currentIndex + 2
                        window.easingType(box.port, easing)
                        plot.easing = easing
                    }
                }

                Components.Label {
                    id: easingTypeLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Acceleration Profile")
                }
            }

            ListModel {
                id: resolutionModel
                ListElement { name: "Coarse" }
                ListElement { name: "Medium" }
                ListElement { name: "Fine" }
            }

            Column {
                id: stepResolutionColumn
                width: Math.max(stepResolutionDropdown.width, stepResolutionLabel.width)
                spacing: 10

                Components.Dropdown {
                    id: stepResolutionDropdown
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 100
                    text: "4"
                    z: 1
                    currentIndex: 0
                    dataModel: resolutionModel
                    onCurrentIndexChanged: window.stepResolution(box.port, currentIndex)
                }

                Components.Label {
                    id: stepResolutionLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Resolution")
                }
            }

            Column {
                id: backlashColumn
                width: Math.max(backlashLine.width, backlashLabel.width)
                spacing: 10

                Components.LineEdit {
                    id: backlashLine
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 70
                    text: "0"
                    validator: IntValidator {
                        top: 300
                        bottom: 0
                    }

                    onAccepted: window.backlash(box.port, parseInt(text))
                }

                Components.Label {
                    id: backlashLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Backlash")
                }
            }

            Column {
                id: powerSaveColumn
                width: Math.max(powerSaveSwitcher.width, powerSaveLabel.width)
                spacing: 10

                Item {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 30; width: 60

                    Components.Switcher {
                        id: powerSaveSwitcher
                        anchors.centerIn: parent
                        width: 60
                        checked: {
                            switch(box.port) {
                            case 1: return window.motor1PowerSave
                            case 2: return window.motor2PowerSave
                            case 3: return window.motor3PowerSave
                            }
                        }

                        property bool first: true
                        onCheckedChanged: {
                            switch(box.port) {
                            case 1: window.motor1PowerSave = checked; break;
                            case 2: window.motor2PowerSave = checked; break;
                            case 3: window.motor3PowerSave = checked; break;
                            }

                            if(first) {
                                first = false
                                return
                            }

                            window.powerSaveMode(box.port, checked)
                        }
                    }
                }

                Components.Label {
                    id: powerSaveLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Power Save")
                }
            }

            Column {
                id: invertDirectionColumn
                width: Math.max(invertDirectionSwitcher.width, invertDirectionLabel.width)
                spacing: 10

                Item {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 30; width: 60

                    Components.Switcher {
                        id: invertDirectionSwitcher
                        anchors.centerIn: parent
                        width: 60
                        onCheckedChanged: window.invertDirection(box.port, checked)
                    }
                }

                Components.Label {
                    id: invertDirectionLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Invert Direction")
                }
            }

            Column {
                id: clearColumn
                width: Math.max(clearButton.width, clearLabel.width)
                spacing: 10

                Components.StandardButton {
                    id: clearButton
                    anchors.horizontalCenter: parent.horizontalCenter
                    image: "qrc:///images/ui/clear.png"
                    width: 30
                    onClicked: {
                        powerSaveSwitcher.checked = true
                        invertDirectionSwitcher.checked = false
                        stepResolutionDropdown.currentIndex = 0
                        stepResolutionDropdown.text = "Coarse"
                        easingTypeDropdown.currentIndex = 0
                        easingTypeDropdown.text = "Quadratic"
                        backlashLine.text = "0"
                        plot.clear()
                        window.backlash(box.port, parseInt(text))
                    }
                }

                Components.Label {
                    id: clearLabel
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Clear")
                }
            }
        }
    }
}
