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

        Repeater {
          id: motorsRepeater
          model: motorsModel
          delegate: Modules.MotorModule {
            width: column.width
            visible: window.motor1Available
            z: motorsRepeater.count - index
          }
        }
      }
    }
  }

  Modules.ConnectionManager {
    id: connectionManager
    anchors.fill: parent
  }

  Modules.SkippedConfiguration {
    id: skippedConfiguration
    anchors.fill: parent
  }

  Modules.ConfigureFirstController {
    id: configureFirstController
    anchors.fill: parent
  }

  Timer {
    id: delayTimer
    interval: 400
    repeat: false
    running: false
    onTriggered: {
      if(window.currentControllerConfiguration === 0)
        configureFirstController.visible = false
      if(++window.currentControllerConfiguration >= window.controllersCount) {
        window.currentControllerConfiguration = 0
        assigningFinished.show()
      }
    }
  }

  Connections {
    target: controller
    ignoreUnknownSignals: true
    onTestControllerFinished: delayTimer.start()
    onActionFinished: if(data === "init") assigningFinished.hide()
  }

  Repeater {
    model: window.controllersCount - 1
    delegate: Modules.ConfigureNextController {
      anchors.fill: rootItem
      address: window.currentControllerConfiguration
      opacity: window.currentControllerConfiguration === index + 1 ? 1 : 0
    }
  }

  Modules.AssigningFinished {
    id: assigningFinished
    anchors.fill: parent
  }
}
