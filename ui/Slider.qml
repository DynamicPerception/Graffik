import QtQuick 2.3

Item {
    id: slider
    property bool autoGoToStart: true
    height: 42

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        height: 20
        border.width: 1
        border.color: "#1B2E60"

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1D3266" }
            GradientStop { position: 0.4; color: "#223A76" }
        }
    }

    Timer {
        id: dampTimer
        interval: 100
        repeat: true
        running: mouseArea.pressed || returnAnimation.running
        onTriggered: window.sliderMoved(handler.x - (slider.autoGoToStart ? handler.centerX : -4))
    }

    Image {
        id: handler
        property int centerX: parent.width / 2 - width / 2

        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 2
        x: slider.autoGoToStart ? centerX : -4
        source: mouseArea.containsMouse ? "qrc:///images/ui/sliderHandlerHovered.png" :
                                          "qrc:///images/ui/sliderHandler.png"

        NumberAnimation {
            id: returnAnimation
            target: handler
            property: "x"
            to: handler.centerX
            duration: 200
            easing.type: Easing.OutSine
            onStopped: window.sliderMoved(handler.x - (slider.autoGoToStart ? handler.centerX : -4))
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            drag.target: handler
            drag.axis: Drag.XAxis
            drag.minimumX: -4
            drag.maximumX: slider.width - width + 4
            onPressed: returnAnimation.stop()
            onReleased: if(slider.autoGoToStart) returnAnimation.restart()
        }
    }
}
