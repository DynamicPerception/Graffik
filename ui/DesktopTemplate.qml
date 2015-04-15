import QtQuick 2.3
import "qrc:///components/ui" as Components
import "qrc:///modules/ui" as Modules

Item {
    id: rootItem

    Components.MainMenu {
        id: mainMenu
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
    }

    Components.ProgressForm {
        id: progressForm
        anchors.fill: parent
        anchors.topMargin: mainMenu.height
        z: 1
        visible: window.progressFormVisible
    }

    Components.MessageBox {
        id: messageBox
        anchors.fill: parent
        z: 2
    }

    Rectangle {
        anchors.fill: parent
        anchors.topMargin: mainMenu.height

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1B3A7E" }
            GradientStop { position: 0.5; color: "#1D69D6" }
            GradientStop { position: 1.0; color: "#1B3A7E" }
        }

        Modules.JoystickModule {
            id: joystick
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: 20
            anchors.rightMargin: 10
        }

        Modules.CameraModule {
            id: video
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 20
        }

        Flickable {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: video.right
            anchors.right: joystick.left
            anchors.topMargin: 10
            anchors.leftMargin: 10
            anchors.bottomMargin: 20
            anchors.rightMargin: 0
            contentHeight: column.height + 115
            clip: true

            Column {
                id: column
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 10
                spacing: 20

                Modules.MotorModule {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    visible: window.motor1Available
                    port: 1
                    z: 2
                }

                Modules.MotorModule {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    visible: window.motor2Available
                    port: 2
                    z: 1
                }

                Modules.MotorModule {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    visible: window.motor3Available
                    port: 3
                    z: 0
                }
            }
        }
    }

    Modules.ConnectionManagerModule {
        id: connectionManager
        anchors.fill: parent
    }
}
