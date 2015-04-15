import QtQuick 2.3

Item {
    id: joystick
    width: 330; height: 330

    Item {
        anchors.fill: parent
        rotation: 45

        Image {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 5
            rotation: 0
            source: {
                if(thumb.anchors.horizontalCenterOffset > 0 && !thumb.centered)
                    return "qrc:///images/ui/outerBgGlow.png"
                return "qrc:///images/ui/outerBgNormal.png"
            }
        }

        Image {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 5
            rotation: 90
            source: {
                if(thumb.anchors.verticalCenterOffset > 0 && !thumb.centered)
                    return "qrc:///images/ui/outerBgGlow.png"
                return "qrc:///images/ui/outerBgNormal.png"
            }
        }

        Image {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.margins: 5
            rotation: 180
            source: {
                if(thumb.anchors.horizontalCenterOffset < 0 && !thumb.centered)
                    return "qrc:///images/ui/outerBgGlow.png"
                return "qrc:///images/ui/outerBgNormal.png"
            }
        }

        Image {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 5
            rotation: 270
            source: {
                if(thumb.anchors.verticalCenterOffset < 0 && !thumb.centered)
                    return "qrc:///images/ui/outerBgGlow.png"
                return "qrc:///images/ui/outerBgNormal.png"
            }
        }
    }

    Image {
        id: innerBgImage
        anchors.centerIn: parent
        source: "qrc:///images/ui/joystickInnerBg.png"

        ParallelAnimation {
            id: returnAnimation
            onStopped: window.joystickMoved(thumb.anchors.horizontalCenterOffset,
                                            thumb.anchors.verticalCenterOffset)

            NumberAnimation {
                target: thumb.anchors
                property: "horizontalCenterOffset"
                to: 0; duration: 200
                easing.type: Easing.OutSine
            }

            NumberAnimation {
                target: thumb.anchors
                property: "verticalCenterOffset"
                to: 0; duration: 200
                easing.type: Easing.OutSine
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            //property real fingerAngle: Math.atan2(mouseX, mouseY)
            property int mcx: mouseX - width * 0.5
            property int mcy: mouseY - height * 0.5
            property bool fingerOutOfBounds: fingerDistance2 < distanceBound2
            property real fingerDistance2: mcx * mcx + mcy * mcy
            property real distanceBound: width * 0.5 - thumb.width * 0.5
            property real distanceBound2: distanceBound * distanceBound

            onPressed: returnAnimation.stop()
            onReleased: returnAnimation.restart()
            onPositionChanged: {
                if(fingerOutOfBounds) {
                    thumb.anchors.horizontalCenterOffset = mcx
                    thumb.anchors.verticalCenterOffset = mcy
                } else {
                    var angle = Math.atan2(mcy, mcx)
                    thumb.anchors.horizontalCenterOffset = Math.cos(angle) * distanceBound
                    thumb.anchors.verticalCenterOffset = Math.sin(angle) * distanceBound
                }
            }
        }

        Image {
            anchors.centerIn: thumb
            anchors.horizontalCenterOffset: -6
            anchors.verticalCenterOffset: 6
            source: "qrc:///images/ui/joystickShadow.png"
        }

        Timer {
            id: dampTimer
            interval: 100
            repeat: true
            running: mouseArea.pressed || returnAnimation.running
            onTriggered: window.joystickMoved(thumb.anchors.horizontalCenterOffset,
                                              thumb.anchors.verticalCenterOffset)
        }

        Image {
            id: thumb
            anchors.centerIn: parent
            source: "qrc:///images/ui/joystickHandler.png"

            property bool centered: anchors.horizontalCenterOffset === 0 &&
                                    anchors.verticalCenterOffset === 0
        }
    }
}
