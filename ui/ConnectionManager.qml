import QtQuick 2.3
import "qrc:///components/ui" as Components

Item {
  id: form
  visible: opacity !== 0

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
    width: 450; height: 130
    color: "#242424"
    radius: 5

    Grid {
      id: row
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.top: parent.top
      anchors.margins: 14
      columns: 3
      columnSpacing: 14
      rowSpacing: 4
      z: 1

      property int buttonWidth: (width - 85 - 3 * columnSpacing) / 2
      Components.LineEdit {
        id: controllersCountEdit
        width: 100
        validator: IntValidator { bottom: 1; top: 255 }
        text: window.controllersCount
        onTextChanged: window.controllersCount = parseInt(text)
      }

      Components.StandardButton {
        id: configureButton
        width: parent.buttonWidth
        text: qsTr("Configure Controlls")
        onClicked: {
          form.hide()
          configureFirstController.show()
        }
      }

      Components.StandardButton {
        id: skipButton
        width: parent.buttonWidth
        text: qsTr("Skip Configuration")
        onClicked: {
          form.hide()
          skippedConfiguration.show()
        }
      }

      Components.Label {
        width: controllersCountEdit.width
        text: "# of Controllers"
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

      text: qsTr("Choose <font color='#FFFFFF'><b>Configure Controlls</b></font> option if you have not configured this exact group of controllers before")
    }
  }
}
