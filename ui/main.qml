import QtQuick 2.3
import QtQuick.Controls 1.2

ApplicationWindow {
  id: window
  objectName: "appWindow"

  visible: true
  minimumWidth: 1280
  maximumWidth: 1280
  minimumHeight: 737
  height: 737
  title: qsTr("Graffik")

  /*
        programStatus === 0 - stopped
        programStatus === 1 - running
        programStatus === 2 - paused
        programStatus === 3 - going to start position
    */

  property string portName: ""
  property int programStatus: 0
  property int programProgress: 0
  property int currentControllerConfiguration: 0
  property int currentControllerAddress: 3
  property int controllersCount: 3
  property bool progressFormVisible: false

  property bool pingPong: false
  property bool continuous: true
  property bool moveShoot: false
  property bool connected: false
  property int videoLengthMinutes: 0
  property real videoLengthSeconds: 12.5
  property int shootingHours: 00
  property int shootingMinutes: 20
  property int shootingSecs: 00
  property var availablePorts
  property bool motorsOnStartPositions: false

  property real focusTime: 0.6
  property real triggerTime: 0.1
  property real exposureDelay: 0.8
  property real interval: 4.0
  property real exposure: 1.5
  property int programMode: 0
  property int videoFrames: 300

  signal startProgramClicked()
  signal stopProgramClicked()
  signal pauseProgramClicked()
  signal goToProgramStartClicked()

  signal clearClicked(int motor)
  signal joystickMoved(int xpos, int ypos)
  signal dampingChanged(real pos)
  signal maxJoystickSpeedChanged(real pos)
  signal sliderMoved(int xpos)
  signal motionChanged(int address, int motor, real p1, real p2, real p3, real p4)
  signal connectToPortClicked(string port, string address)
  signal portsRescanClicked()
  signal checkMotorAttachmentClicked()
  signal validateRequest(int address)
  signal closePort()
  signal assignAddressRequest(string port, int address)
  signal connectToDevices()

  onSliderMoved: window.motorsOnStartPositions = false
  onJoystickMoved: window.motorsOnStartPositions = false

  property var currentItem
  function mouseReleased(obj, mouseX, mouseY) {}
  function mousePressed(obj, mouseX, mouseY) {
    switch(obj.objectName) {
    case "appWindow": obj = rootItem; break;
    case "popupWindow": return
    }

    var tmp = rootItem.mapFromItem(obj, mouseX, mouseY)
    if(tmp !== undefined) {
      mouseX = tmp.x
      mouseY = tmp.y
    }

    if(currentItem !== undefined) {
      tmp = rootItem.mapToItem(currentItem, mouseX, mouseY)
      if(tmp !== null && !currentItem.containsPoint(tmp.x, tmp.y)) {
        currentItem.hide()
        currentItem = undefined
      }
    }
  }

  FontLoader { id: fontLight; source: "qrc:///fonts/ui/HNLight.ttf" }
  FontLoader { id: fontRoman; source: "qrc:///fonts/ui/HNRoman.ttf" }

  Item {
    id: rootItem
    anchors.fill: parent

    Loader {
      anchors.fill: parent
      source: "qrc:///ui/DesktopTemplate.qml"
    }
  }
}


