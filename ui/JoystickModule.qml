import QtQuick 2.3
import "qrc:///components/ui" as Components

Item {
  id: module
  width: 300

  Image {
    anchors.left: parent.left
    anchors.top: parent.top
    z: 1
    source: setStartArea.pressed || startTimer.running ? "qrc:///images/ui/setStartPressed.png" :
                                                         "qrc:///images/ui/setStart.png"

    Timer {
      id: startTimer
      running: false
      repeat: false
      interval: 1000
    }

    MouseArea {
      id: setStartArea
      anchors.fill: parent
      onClicked: {
        startTimer.restart()
        controller.setProgramStartPoint()
      }
    }
  }

  Image {
    anchors.right: parent.right
    anchors.top: parent.top
    z: 1
    source: setStopArea.pressed || stopTimer.running ? "qrc:///images/ui/setStopPressed.png" :
                                                       "qrc:///images/ui/setStop.png"

    Timer {
      id: stopTimer
      running: false
      repeat: false
      interval: 1000
    }

    MouseArea {
      id: setStopArea
      anchors.fill: parent
      onClicked: {
        stopTimer.restart()
        controller.setProgramStopPoint()
      }
    }
  }

  Components.Joystick {
    id: joystick
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: -15
  }

  Components.Slider {
    id: slider
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: joystick.bottom
    anchors.topMargin: 10
    anchors.leftMargin: 10
    anchors.rightMargin: 10
  }

  Components.Box {
    anchors.top: slider.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.margins: 10
    height: column.height + 46

    Column {
      id: column
      anchors.top: parent.top
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.margins: 10
      anchors.topMargin: 13
      spacing: 10

      Item {
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30

        Components.Label {
          id: advLabel
          anchors.left: parent.left
          anchors.verticalCenter: parent.verticalCenter
          height: 20
          font.pixelSize: 16
          verticalAlignment: Text.AlignVCenter
          text: qsTr("Address %1").arg(window.currentControllerAddress - 2)
        }

        Components.StandardButton {
          anchors.right: parent.right
          anchors.verticalCenter: parent.verticalCenter
          width: 80
          fontSize: 14
          text: qsTr("Change")
          onClicked: {
            if(window.currentControllerAddress >= window.controllersCount + 2) {
              window.currentControllerAddress = 3
              controller.setDeviceAddress(window.currentControllerAddress)
            } else {
              controller.setDeviceAddress(++window.currentControllerAddress)
            }
          }
        }
      }

      Components.VerticalSeparator { width: parent.width }

      Components.StyledSlider {
        anchors.left: parent.left
        anchors.right: parent.right
        title: qsTr("Joystick Speed")
        from: qsTr("Slow")
        to: qsTr("Fast")
        onMoved: window.maxJoystickSpeedChanged(position)
      }

      Components.StyledSlider {
        anchors.left: parent.left
        anchors.right: parent.right
        title: qsTr("Damping")
        from: qsTr("Slow")
        to: qsTr("Fast")
        onMoved: window.dampingChanged(position)
      }
    }
  }
}
