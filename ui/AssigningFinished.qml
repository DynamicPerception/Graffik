import QtQuick 2.3
import "qrc:///components/ui" as Components

Item {
  id: form
  visible: opacity !== 0
  opacity: 0

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
    onStarted: nextButton.enabled = true
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

    Components.StandardButton {
      id: nextButton
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.top: parent.top
      anchors.margins: 14
      width: 100
      text: qsTr("Next")
      onClicked: {
        window.connectToDevices()
        nextButton.enabled = false
      }
    }

    Components.VerticalSeparator {
      id: separator
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: nextButton.bottom
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

      text: qsTr("You <font color='#FFFFFF'><b>succcessfully</b></font> connected device")
    }
  }
}
