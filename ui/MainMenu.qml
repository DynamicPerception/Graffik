import QtQuick 2.3
import "qrc:///components/ui" as Components

Rectangle {
    height: 60
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#242424" }
        GradientStop { position: 1.0; color: "#0D0D0B" }
    }

    Item {
        width: 60; height: 60
        x: 0; y: 0
        Image {
            anchors.centerIn: parent
            source: "qrc:///images/ui/logo.png"
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: Qt.openUrlExternally("http://dynamicperception.com")
        }
    }

    Row {
        anchors.centerIn: parent
        spacing: 20

        // Version Number
        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 3

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Arial"
                font.pixelSize: 16
                renderType: Text.NativeRendering
                color: "#FFFFFF"
                text: qsTr("0.10")
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Arial"
                font.pixelSize: 13
                renderType: Text.NativeRendering
                color: "#808080"
                text: qsTr("Version")
            }
        }

        // Current Time
        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 3

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Arial"
                font.pixelSize: 16
                renderType: Text.NativeRendering
                color: "#FFFFFF"
                text: "00:00:00"

                Timer {
                    running: true
                    interval: 1000
                    repeat: true
                    triggeredOnStart: true

                    onTriggered: {
                        var d = new Date()
                        var h = d.getHours()
                        var m = d.getMinutes()
                        var s = d.getSeconds()

                        h = h < 10 ? "0" + h : h
                        m = m < 10 ? "0" + m : m
                        s = s < 10 ? "0" + s : s
                        parent.text = h + ":" + m + ":" + s
                    }
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Arial"
                font.pixelSize: 13
                renderType: Text.NativeRendering
                color: "#808080"
                text: qsTr("Current Time")
            }
        }

        Image {
            anchors.verticalCenter: parent.verticalCenter
            source: {
                if((window.programStatus === 0 || window.programStatus === 3) && !stopArea.pressed && !playArea.pressed && !window.motorsOnStartPositions)
                    return "qrc:///images/ui/playerState1.png"
                if((window.programStatus === 0 || window.programStatus === 3) && playArea.pressed && !window.motorsOnStartPositions)
                    return "qrc:///images/ui/playerState2.png"
                if((window.programStatus === 0 || window.programStatus === 3) && stopArea.pressed && !window.motorsOnStartPositions)
                    return "qrc:///images/ui/playerState3.png"
                if((window.programStatus === 0 || window.programStatus === 2) && !stopArea.pressed && !playArea.pressed && window.motorsOnStartPositions)
                    return "qrc:///images/ui/playerState4.png"
                if((window.programStatus === 0 || window.programStatus === 2) && playArea.pressed && window.motorsOnStartPositions)
                    return "qrc:///images/ui/playerState5.png"
                if((window.programStatus === 0 || window.programStatus === 2) && stopArea.pressed && window.motorsOnStartPositions)
                    return "qrc:///images/ui/playerState6.png"
                if(window.programStatus === 1 && !stopArea.pressed && !playArea.pressed)
                    return "qrc:///images/ui/playerState7.png"
                if(window.programStatus === 1 && stopArea.pressed)
                    return "qrc:///images/ui/playerState8.png"
                if(window.programStatus === 1 && playArea.pressed)
                    return "qrc:///images/ui/playerState7.png"
                return ""
            }

            /*
                window.programStatus === 0 - stopped
                window.programStatus === 1 - running
                window.programStatus === 2 - paused
                window.programStatus === 3 - going to start position
            */

            MouseArea {
                id: stopArea
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                width: parent.width / 2
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    window.motorsOnStartPositions = false
                    window.stopProgramClicked()
                    window.progressFormVisible = false
                }
            }

            MouseArea {
                id: playArea
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: parent.width / 2
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    console.log("window.programStatus: " + window.programStatus +
                                "  window.motorsOnStartPositions: " + window.motorsOnStartPositions)
                    if((window.programStatus === 0 ||
                       window.programStatus === 2) &&
                       window.motorsOnStartPositions)
                    {                        
                        if(window.motor1Valid && window.motor2Valid && window.motor3Valid) {
                            window.startProgramClicked()
                        } else {
                            messageBox.content = qsTr("Program cannont be completed with maximum available motor speed. Please increase buffer time or frames, or adjust start/stop points.")
                            messageBox.show()
                        }
                    } else if(window.programStatus === 1) {
                        window.pauseProgramClicked()
                    } else {
                        if(window.motor1Valid && window.motor2Valid && window.motor3Valid) {
                            window.goToProgramStartClicked()
                        } else {
                            messageBox.content = qsTr("Program cannont be completed with maximum available motor speed. Please increase buffer time or frames, or adjust start/stop points.")
                            messageBox.show()
                        }
                    }
                }
            }

//            MouseArea {
//                anchors.top: parent.top
//                anchors.bottom: parent.bottom
//                anchors.right: parent.right
//                cursorShape: Qt.PointingHandCursor
//                width: 46
//                onClicked: window.goToProgramStartClicked()
//            }
        }
    }

    Row {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        MenuButton {
            checkedImage: "qrc:///images/ui/attachmentTestPressed.png"
            uncheckedImage: "qrc:///images/ui/attachmentTestNormal.png"
            enabled: !window.progressFormVisible
            text: qsTr("Check")
            onClicked: window.checkMotorAttachmentClicked()
        }

        MenuButton {
            checkedImage: "qrc:///images/ui/menuSettingsChecked.png"
            uncheckedImage: "qrc:///images/ui/menuSettingsUnchecked.png"
            enabled: !window.progressFormVisible
            text: qsTr("Settings")
            onClicked: {
                window.closePort()
                window.connected = false
            }
        }

        MenuButton {
            checkedImage: "qrc:///images/ui/menuSupportChecked.png"
            uncheckedImage: "qrc:///images/ui/menuSupportUnchecked.png"
            enabled: !window.progressFormVisible
            text: qsTr("Support")
            onClicked: Qt.openUrlExternally("http://support.dynamicperception.com/hc/en-us")
        }
    }
}
