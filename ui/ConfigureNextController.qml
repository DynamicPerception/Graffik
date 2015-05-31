import QtQuick 2.3
import "qrc:///components/ui" as Components

Item {
  id: form
  visible: opacity !== 0
  opacity: 0

  property int address: 0
  function show() { showAnimation.start() }
  function hide() { hideAnimation.start() }

  NumberAnimation {
    id: hideAnimation
    target: form
    properties: "opacity"
    to: 0
    from: 1
    duration: 100
    easing.type: Easing.InQuad
  }

  NumberAnimation {
    id: showAnimation
    target: form
    properties: "opacity"
    to: 1
    from: 0
    duration: 100
    easing.type: Easing.InQuad
  }

  Rectangle {
    anchors.fill: parent
    color: "#FFFFFF"
    opacity: 0.5
  }

  MouseArea {
    anchors.fill: parent
    onPressed: {}
    onReleased: {}
  }

  Rectangle {
    anchors.centerIn: parent
    width: 520; height: 135
    color: "#242424"
    radius: 5

    Grid {
      id: row
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: parent.top
      anchors.margins: 14
      columns: 2
      columnSpacing: 14
      rowSpacing: 4
      z: 1

      Components.StandardButton {
        id: assignButton
        width: (parent.width - 14) / 2
        text: qsTr("Assign Controller %1").arg(form.address + 1)
        onClicked: {
          assignButton.enabled = false
          cancelButton.enabled = false
          window.assignAddressRequest(window.portName, form.address + 3)
        }
      }

      Components.StandardButton {
        id: cancelButton
        width: (parent.width - 14) / 2
        text: qsTr("Cancel Configuration")
        onClicked: {
          window.currentControllerConfiguration = 0
          form.hide()
          connectionManager.show()
        }
      }
    }

    Components.VerticalSeparator {
      id: separator
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: row.bottom
      anchors.margins: 14
    }

    Text {
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: parent.bottom
      anchors.top: separator.bottom
      anchors.margins: 14
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter

      font.family: fontRoman.name
      font.pixelSize: 14
      color: "#7F7F7F"
      wrapMode: Text.Wrap

      text: qsTr("Unplug USB cable from controller and connect next unassigned controller. f you are using a desktop or laptop, do not move the cable to a different USB port on your host device.")
    }
  }
}
