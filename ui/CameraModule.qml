import QtQuick 2.3
import "qrc:///components/ui" as Components

Components.Box {
  id: box
  objectName: "videoModule"
  property int prevProgramMode: 0
  property bool completed: false
  clip: false
  height: timelapseColumn.y + timelapseColumn.height + 20

  Component.onCompleted: completed = true


  /*******************************
      Program Calculator Functions
    ********************************/

  function round(value) {
    var n = Math.round(parseFloat(value) * 10) / 10
    var s = '' + n
    var dotIndex = s.indexOf(".")
    s = dotIndex === 0 ? "0" + s : s
    s = dotIndex === -1 ? s + ".0" : s
    return s
  }

  function setShootingDuration(secs) {
    if(!completed) return

    var h = parseInt(secs / 3600)
    var m = parseInt(secs / 60) % 60
    var s = parseInt(secs % 60)

    hoursLine.text = h < 10 ? "0" + h : h
    minutesLine.text = m < 10 ? "0" + m : m
    secondsLine.text = s < 10 ? "0" + s : s

    window.shootingHours = hoursLine.text
    window.shootingMinutes = minutesLine.text
    window.shootingSecs = secondsLine.text
    window.validateRequest()
  }

  function setVideoLength(secs) {
    if(!completed) return

    var m = parseInt(secs / 60) % 60
    var s = Math.round((secs % 60.0) * 10) / 10

    window.videoLengthMinutes = m
    window.videoLengthSeconds = s
    lengthLineCompiledMinutes.text = m
    lengthLineCompiledSeconds.text = s < 10 ? "0" + s : s
    lengthLineVideoMinutes.text = lengthLineCompiledMinutes.text
    lengthLineVideoSeconds.text = lengthLineVideoMinutes.text
    window.validateRequest()
  }

  function exposureChanged() {
    if(!completed) return

    var exposure = parseFloat(exposureLine.text)
    var interval = parseFloat(intervalLine.text)
    var trigger = parseFloat(triggerLine.text)
    var focus  = parseFloat(focusLine.text )

    if(exposure < (trigger + focus))
      exposure = (trigger + focus)
    if(exposure > interval)
      interval = exposure

    var delay = exposure - trigger - focus

    exposureLine.text = box.round(exposure)
    exposureDelayLine.text = box.round(delay)
    bufferLine.text = box.round(interval - exposure)

    window.exposure = exposure
    window.triggerTime = trigger
    window.exposureDelay = delay
    intervalLine.text = box.round(interval)
    window.validateRequest()
  }

  function advancedSettingsChanged() {
    if(!completed) return

    var interval = parseFloat(intervalLine.text)
    var trigger = parseFloat(triggerLine.text)
    var focus  = parseFloat(focusLine.text )
    var delay = parseFloat(exposureDelayLine.text)
    var exposure = focus + trigger + delay

    if(exposure > interval)
      interval = exposure

    intervalLine.text = box.round(interval)
    exposureLine.text = box.round(exposure)
    focusLine.text = box.round(focus)
    triggerLine.text = box.round(trigger)
    bufferLine.text = box.round(interval - exposure)
    exposureDelayLine.text = box.round(delay)

    window.focusTime = focus
    window.triggerTime = trigger
    window.exposureDelay = delay

    window.validateRequest()
  }

  function focusChanged() {
    if(!completed) return

    var buffer = parseFloat(bufferLine.text)
    var focus = parseFloat(focusLine.text)
    var interval = parseFloat(intervalLine.text)
    var exposure = parseFloat(exposureLine.text)
    var frames = parseFloat(frameLine.text)

    window.focusTime = focus
    focusLine.text = box.round(focus)

    if(buffer < focus) {
      intervalLine.text = box.round(exposure + focus)
      setShootingDuration(interval * frames)
    } else {
      window.validateRequest()
    }
  }

  function intervalChanged() {
    if(!completed) return

    var focus = parseFloat(focusLine.text)
    var interval = parseFloat(intervalLine.text)
    var exposure = parseFloat(exposureLine.text)
    var frames = parseFloat(frameLine.text)

    if(interval < exposure) {
      interval = exposure
    }

    bufferLine.text = box.round(interval - exposure)
    intervalLine.text = box.round(interval)
    window.interval = interval
    setShootingDuration(interval * frames)
  }

  function framesChanged() {
    if(!completed) return

    var interval = parseFloat(intervalLine.text)
    var frames = parseFloat(frameLine.text)
    var fps = parseInt(fpsDropdown.text)

    if((frames / fps) > 599.99) {
      frames = Math.round(599.99 * fps)
      frameLine.text = parseInt(frames)
    }

    setVideoLength(frames / fps)
    window.videoFrames = frames
    setShootingDuration(interval * frames)
  }

  function shootingDurationChanged() {
    if(!completed) return

    var h = parseInt(hoursLine.text) * 3600
    var m = parseInt(minutesLine.text) * 60
    var s = parseInt(secondsLine.text)

    var shootingDurationSecs = h + m + s
    var interval = parseFloat(intervalLine.text)
    var fps = parseInt(fpsDropdown.text)
    var frames = Math.round(shootingDurationSecs / interval)

    if((frames / fps) > 599.99) {
      frames = Math.round(599.99 * fps)

      frameLine.text = parseInt(frames)
      setShootingDuration(frames * interval)
      setVideoLength(frames / fps)
    } else {
      frameLine.text = parseInt(frames)
      setVideoLength(frames / fps)

      h = parseInt(hoursLine.text)
      m = parseInt(minutesLine.text)
      s = parseInt(secondsLine.text)

      hoursLine.text = h < 10 ? "0" + h : h
      minutesLine.text = m < 10 ? "0" + m : m
      secondsLine.text = s < 10 ? "0" + s : s

      window.shootingHours = hoursLine.text
      window.shootingMinutes = minutesLine.text
      window.shootingSecs = secondsLine.text
      window.validateRequest()
    }
  }

  function videoLengthChanged() {
    if(!completed) return

    var interval = parseFloat(intervalLine.text)
    var fps = parseInt(fpsDropdown.text)

    var m = parseFloat(lengthLineCompiledMinutes.text)
    var s = parseFloat(lengthLineCompiledSeconds.text)
    var frames = Math.round((m * 60 + s) * fps)

    window.videoLengthMinutes = m
    window.videoLengthSeconds = s

    if((frames / fps) > 599.99)
      frames = Math.round(599.99 * fps)

    frameLine.text = parseInt(frames)
    setShootingDuration(interval * frames)
  }

  function fpsChanged() {
    if(!completed) return

    var frames = parseFloat(frameLine.text)
    var fps = parseFloat(fpsDropdown.text)

    setVideoLength(frames / fps)
  }


  /*******************************
             UI Componenets
    ********************************/

  // Frames per second (FPS) dropdown elements
  ListModel {
    id: fpsModel
    ListElement { name: "24" }
    ListElement { name: "25" }
    ListElement { name: "30" }
  }

  Image {

    // Timelapse / Video selector switch
    id: modeButtons
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: 10
    source: {
      if(window.programMode === 2)
        return "qrc:///images/ui/modeVideo.png"
      if(window.programMode === 1 || window.programMode === 0)
        return "qrc:///images/ui/modeTimelapse.png"
    }

    // Timelapse switch parameters
    Item {
      anchors.fill: parent
      anchors.leftMargin: parent.width / 2

      Text {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 8
        font.family: "Arial"
        font.pixelSize: 16
        //font.bold: true
        renderType: Text.NativeRendering
        color: "#FFFFFF"
        text: qsTr("Timelapse")
      }

      MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: {
          controller.setCameraEnabled(true)
          if(window.programMode == 2) {
            window.programMode = box.prevProgramMode
          }
        }
      }
    }

    // Video switch parameters
    Item {
      anchors.fill: parent
      anchors.rightMargin: parent.width / 2

      Text {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 40
        font.family: "Arial"
        font.pixelSize: 16
        //font.bold: true
        renderType: Text.NativeRendering
        color: "#FFFFFF"
        text: qsTr("Video")
      }

      MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor

        onClicked: {
          controller.setCameraEnabled(false)
          box.prevProgramMode = window.programMode
          window.programMode = 2
        }
      }
    }
  }

  // Video program tab components
  Column {
    id: videoColumn
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: modeButtons.bottom
    anchors.margins: 20
    spacing: 10
    onOpacityChanged: visible = opacity !== 0
    opacity: window.programMode === 2 ? 1 : 0

    Behavior on opacity {
      NumberAnimation { duration: 100; easing.type: Easing.InQuad }
    }

    Column {
      anchors.left: parent.left
      anchors.right: parent.right
      spacing: 10

      Row {
        anchors.left: parent.left
        anchors.right: parent.right

        property int itemWidth: (width - 40) / 3

        // Video length minutes
        Components.LabeledLineEdit {
          id: lengthLineVideoMinutes
          width: parent.itemWidth
          validator: IntValidator { bottom: 0; top: 9 }
          text: window.videoLengthMinutes
          label: "M"
          onAccepted: {
            lengthLineCompiledMinutes.text = text
            box.videoLengthChanged()
          }
        }

        Item {
          width: 20; height: 30
          Text {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -2
            font.family: "Arial"
            font.pixelSize: 16
            color: "#818181"
            text: ":"
          }
        }

        // Video length seconds
        Components.LabeledLineEdit {
          id: lengthLineVideoSeconds
          width: parent.itemWidth
          validator: DoubleValidator {
            notation: DoubleValidator.StandardNotation
            locale: "C"
            decimals: 1
          }
          minValue: 0; maxValue: 59.99
          validationEnabled: true
          text: box.round(window.videoLengthSeconds)
          label: "S"
          onAccepted: {
            lengthLineCompiledSeconds.text = text
            box.videoLengthChanged()
          }
        }

        Item { width: 5; height: 1 } //separator

        Components.Label {
          anchors.verticalCenter: parent.verticalCenter
          width: parent.width - (2 * parent.itemWidth + 20)
          text: qsTr("Video Duration")
        }
      }

      Item {
        height: 30
        width: parent.width - x

        Components.RadioButton {
          id: oneShotButton
          anchors.verticalCenter: parent.verticalCenter
          anchors.left: parent.left
          anchors.right: parent.right
          text: qsTr("One Shot")
          checked: true
          onClicked: {
            window.pingPong = false
            pingPongButton.checked = false
          }
        }
      }

      Components.RadioButton {
        id: pingPongButton
        width: parent.width - x
        text: qsTr("Ping-Pong")
        onClicked: {
          window.pingPong = true
          oneShotButton.checked = false
        }
      }
    }
  } // End video tab components

  // Timelapse tab components
  Column {
    id: timelapseColumn
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: modeButtons.bottom
    anchors.margins: 20
    spacing: 10
    onOpacityChanged: visible = opacity !== 0
    opacity: window.programMode === 1 || window.programMode === 0 ? 1 : 0

    Behavior on opacity {
      NumberAnimation { duration: 100; easing.type: Easing.InQuad }
    }

    // Continuous TL mode button
    Components.RadioButton {
      id: continuousButton
      anchors.left: parent.left
      anchors.right: parent.right
      text: qsTr("Continuous")
      checked: false
      onClicked: {
        window.programMode = 1
        moveShootButton.checked = false
      }
    }

    // SMS mode button
    Components.RadioButton {
      id: moveShootButton
      anchors.left: parent.left
      anchors.right: parent.right
      text: qsTr("Move Shoot")
      checked: true
      onClicked: {
        window.programMode = 0
        continuousButton.checked = false
      }
    }


    Grid {
      anchors.left: parent.left
      anchors.right: parent.right
      rowSpacing: 5
      columns: 5

      property int itemWidth: (width - 30) / 3

      // Exposuse, buffer, interval input line

      // Exposure input
      Components.LineEdit {
        id: exposureLine
        width: parent.itemWidth
        text: box.round(window.exposure)
        validator: DoubleValidator {
          notation: DoubleValidator.StandardNotation
          locale: "C"
          decimals: 1
        }

        maxValue: 60; minValue: 0
        validationEnabled: true
        onAccepted: box.exposureChanged()
      }

      // Separator
      Item { height: 30; width: 10 }

      // Buffer display
      Components.LineEdit {
        id: bufferLine
        width: parent.itemWidth
        readOnly: true
        text: "2.5"
        //validator: IntValidator {}
      }

      Item {
        width: 20; height: 30
        Text {
          anchors.centerIn: parent
          anchors.verticalCenterOffset: -2
          font.family: "Arial"
          font.pixelSize: 16
          color: "#818181"
          text: "="
        }
      }

      // Interval input
      Components.LineEdit {
        id: intervalLine
        width: parent.itemWidth
        text: box.round(window.interval)
        validator: DoubleValidator {
          notation: DoubleValidator.StandardNotation
          locale: "C"
          decimals: 1
        }

        maxValue: 10500; minValue: 0
        validationEnabled: true
        onAccepted: box.intervalChanged()
      }

      // Exposure, buffer, interval label line

      // Exposure label
      Components.Label {
        horizontalAlignment: Text.AlignHCenter
        width: parent.itemWidth
        text: qsTr("Exposure")
      }

      // Separator
      Item { height: 1; width: 10 }

      // Buffer label
      Components.Label {
        horizontalAlignment: Text.AlignHCenter
        width: parent.itemWidth
        text: qsTr("Buffer")
      }

      // Separator
      Item { height: 1; width: 20 }

      // Interval label
      Components.Label {
        horizontalAlignment: Text.AlignHCenter
        width: parent.itemWidth
        text: qsTr("Interval")
      }
    }

    // Separator line
    Components.VerticalSeparator { width: parent.width }

    // Advanced settings componetnts
    Item {
      width: parent.width
      clip: true
      height: {
        if(advSwitcher.checked)
          return advGrid.y + advGrid.height
        return advSwitcher.height
      }

      Behavior on height {
        NumberAnimation { duration: 100; easing.type: Easing.InQuad }
      }

      // Advanced settings label
      Components.Label {
        id: advLabel
        anchors.left: parent.left
        anchors.top: parent.top
        height: 20
        font.pixelSize: 16
        verticalAlignment: Text.AlignVCenter
        text: qsTr("Advanced Settings")
      }

      // Advanced settings switch
      Components.Switcher {
        id: advSwitcher
        anchors.right: parent.right
        anchors.top: parent.top
        onClicked: {
          if(!checked) {
            window.focusTime = 0.6
            window.triggerTime = 0.1
            window.exposureDelay = 0.8

            focusLine.text = window.focusTime
            triggerLine.text = window.triggerTime
            exposureDelayLine.text = window.exposureDelay
            box.advancedSettingsChanged()
          }
        }
      }

      // Advanced settings input
      Grid {
        id: advGrid
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: Math.max(advSwitcher.height, advLabel.height) + 10
        columns: 3
        spacing: 10

        property int itemWidth: (width - 20) / 3

        // Focus input box
        Components.LineEdit {
          id: focusLine
          width: parent.itemWidth
          text: box.round(window.focusTime)
          validator: DoubleValidator {
            notation: DoubleValidator.StandardNotation
            locale: "C"
            decimals: 1
          }

          maxValue: 60; minValue: 0
          validationEnabled: true
          onAccepted: box.advancedSettingsChanged()
        }

        // Trigger input box
        Components.LineEdit {
          id: triggerLine
          width: parent.itemWidth
          text: box.round(window.triggerTime)
          validator: DoubleValidator {
            notation: DoubleValidator.StandardNotation
            locale: "C"
            decimals: 1
          }

          maxValue: 10000; minValue: 0
          validationEnabled: true
          onAccepted: box.advancedSettingsChanged()
        }

        // Exposure delay input box
        Components.LineEdit {
          id: exposureDelayLine
          width: parent.itemWidth
          text: box.round(window.exposureDelay)
          validator: DoubleValidator {
            notation: DoubleValidator.StandardNotation
            locale: "C"
            decimals: 1
          }

          maxValue: 60; minValue: 0
          validationEnabled: true
          onAccepted: box.advancedSettingsChanged()
        }

        // Focus label
        Components.Label {
          horizontalAlignment: Text.AlignHCenter
          width: parent.itemWidth
          text: qsTr("Focus")
        }

        // Trigger label
        Components.Label {
          horizontalAlignment: Text.AlignHCenter
          width: parent.itemWidth
          text: qsTr("Trigger")
        }

        // Exposure delay label
        Components.Label {
          horizontalAlignment: Text.AlignHCenter
          width: parent.itemWidth
          text: qsTr("Expose Delay")
        }
      }
    }

    // Separator line
    Components.VerticalSeparator { width: parent.width }

    // TL shooting duration label
    Components.Label {
      anchors.horizontalCenter: parent.horizontalCenter
      font.pixelSize: 16
      text: qsTr("Shooting Duration")
    }

    // TL shooting duration input boxes
    Row {
      anchors.left: parent.left
      anchors.right: parent.right

      property int itemWidth: (width - 40) / 3

      // TL duration hours input
      Components.LabeledLineEdit {
        id: hoursLine
        width: parent.itemWidth
        validator: IntValidator { bottom: 0 }
        text: window.shootingHours
        label: "H"
        onAccepted: box.shootingDurationChanged()
      }

      // ":" Separator
      Item {
        width: 20; height: 30
        Text {
          anchors.centerIn: parent
          anchors.verticalCenterOffset: -2
          font.family: "Arial"
          font.pixelSize: 16
          color: "#818181"
          text: ":"
        }
      }

      // TL duration minutes input
      Components.LabeledLineEdit {
        id: minutesLine
        width: parent.itemWidth
        validator: IntValidator { top: 59; bottom: 0 }
        text: window.shootingMinutes
        label: "M"
        onAccepted: box.shootingDurationChanged()
      }

      // ":" Separator
      Item {
        width: 20; height: 30
        Text {
          anchors.centerIn: parent
          anchors.verticalCenterOffset: -2
          font.family: "Arial"
          font.pixelSize: 16
          color: "#818181"
          text: ":"
        }
      }

      // TL duration seconds input
      Components.LabeledLineEdit {
        id: secondsLine
        width: parent.itemWidth
        validator: IntValidator { top: 59; bottom: 0 }
        text: window.shootingSecs
        label: "S"
        onAccepted: box.shootingDurationChanged()
      }
    }

    // Separator
    Components.VerticalSeparator { width: parent.width }

    // Output label
    Components.Label {
      anchors.horizontalCenter: parent.horizontalCenter
      font.pixelSize: 16
      text: qsTr("Output")
    }

    // Frames / FPS input
    Grid {
      anchors.horizontalCenter: parent.horizontalCenter
      columns: 2
      columnSpacing: 20
      rowSpacing: 10
      z: 1

      // Frames input box
      Components.LineEdit {
        id: frameLine
        width: 70
        validator: IntValidator { bottom: 0 }
        text: window.videoFrames
        onAccepted: box.framesChanged()
      }

      // FPS dropdown menu
      Components.Dropdown {
        id: fpsDropdown
        width: 60
        z: 1
        dataModel: fpsModel
        text: "24"
        onCurrentIndexChanged: box.fpsChanged()
      }

      // Frames label
      Components.Label {
        width: 70
        text: qsTr("Frames")
      }

      // FPS label
      Components.Label {
        width: 60
        text: qsTr("FPS")
      }
    }

    // Separator
    Components.VerticalSeparator { width: parent.width }

    // Compiled video duration label
    Components.Label {
      anchors.horizontalCenter: parent.horizontalCenter
      font.pixelSize: 16
      text: qsTr("Video Duration")
    }

    // Compiled video duration input line
    Column {
      id: videoLengthColumn
      anchors.left: parent.left
      anchors.right: parent.right
      property int itemWidth: (width - 40) / 3

      Row {
        anchors.horizontalCenter: parent.horizontalCenter

        // Compiled video duration minutes input
        Components.LabeledLineEdit {
          id: lengthLineCompiledMinutes
          width: videoLengthColumn.itemWidth
          validator: IntValidator { bottom: 0; top: 9 }
          text: window.videoLengthMinutes
          label: "M"
          onAccepted: {
            lengthLineVideoMinutes.text = text
            box.videoLengthChanged()
          }
        }

        // ":" separator
        Item {
          width: 20; height: 30
          Text {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -2
            font.family: "Arial"
            font.pixelSize: 16
            color: "#818181"
            text: ":"
          }
        }

        // Compiled video duration seconds input
        Components.LabeledLineEdit {
          id: lengthLineCompiledSeconds
          width: 80
          validator: DoubleValidator {
            notation: DoubleValidator.StandardNotation
            locale: "C"
            decimals: 1
          }
          minValue: 0; maxValue: 59.9
          validationEnabled: true
          text: box.round(window.videoLengthSeconds)
          label: "S"
          onAccepted: {
            lengthLineVideoSeconds.text = text
            box.videoLengthChanged()
          }
        }
      }
    }
  }   // End timelapse tab components
}
