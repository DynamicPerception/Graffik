import QtQuick 2.3
import QtQuick.Controls 1.2

Item {
    id: plot
    property int easing: 2
    property bool error: false

    onErrorChanged: canvas.requestPaint()
    onEasingChanged: canvas.requestPaint()
    clip: true

    function repaint() {
        canvas.requestPaint()
    }

    function clear() {
        point1.x = -5
        point2.x = 200
        point3.x = background.width - 200 - point3.width
        point4.x = background.width - point4.width + 5

        plot.changeMotion()
        canvas.requestPaint()
    }

    function changeMotion() {
        var w = (background.width + 5)
        var p1 = (point1.x + 5) / w
        var p2 = (point2.x + 5) / w
        var p3 = (point3.x + 5) / w
        var p4 = (point4.x + 5) / w
        window.motionChanged(deviceAddress, motorPort, p1, p2, p3, p4)
    }

    Image {
        id: background
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        source: "qrc:///images/ui/grid.png"
        y: 10

        Rectangle {
            id: point1
            x: -5; y: background.height - 5
            width: 10; height: 10
            radius: 5
            color: "#FFFFFF"
            z: x + 5 > parent.width / 2 ? 2 : 1

            MouseArea {
                anchors.centerIn: parent
                width: 40; height: 40
                drag.target: point1
                drag.axis: Drag.XAxis
                drag.minimumX: -5
                drag.maximumX: point2.x
                onPositionChanged: canvas.requestPaint()
                onReleased: plot.changeMotion()
            }
        }

        Rectangle {
            id: point2
            x: 200; y: -5
            width: 10; height: 10
            radius: 5
            color: "#FFFFFF"
            z: x + 5 > parent.width / 2 ? 2 : 1

            MouseArea {
                anchors.centerIn: parent
                width: 40; height: 40
                drag.target: point2
                drag.axis: Drag.XAxis
                drag.minimumX: point1.x
                drag.maximumX: point3.x
                onPositionChanged: canvas.requestPaint()
                onReleased: plot.changeMotion()
            }
        }

        Rectangle {
            id: point3
            x: parent.width - 200 - width; y: -5
            width: 10; height: 10
            radius: 5
            color: "#FFFFFF"
            z: x + 5 < parent.width / 2 ? 2 : 1

            MouseArea {
                anchors.centerIn: parent
                width: 40; height: 40
                drag.target: point3
                drag.axis: Drag.XAxis
                drag.minimumX: point2.x
                drag.maximumX: point4.x
                onPositionChanged: canvas.requestPaint()
                onReleased: plot.changeMotion()
            }
        }

        Rectangle {
            id: point4
            x: parent.width - width + 5; y: background.height - 5
            width: 10; height: 10
            radius: 5
            color: "#FFFFFF"
            z: x + 5 < parent.width / 2 ? 2 : 1

            MouseArea {
                anchors.centerIn: parent
                width: 40; height: 40
                drag.target: point4
                drag.axis: Drag.XAxis
                drag.minimumX: point3.x
                drag.maximumX: background.width - 5
                onPositionChanged: canvas.requestPaint()
                onReleased: plot.changeMotion()
            }
        }

        Canvas {
            id: canvas
            anchors.fill: parent
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                ctx.lineWidth = 2
                ctx.strokeStyle = plot.error ? "#D90000" : "#007AFF"

                var s = Qt.point(0, height - 1)
                var e = Qt.point(width, height - 1)
                var p1 = Qt.point(point1.x + 5, point1.y + 4)
                var p2 = Qt.point(point2.x + 5, point2.y + 6)
                var p3 = Qt.point(point3.x + 5, point3.y + 6)
                var p4 = Qt.point(point4.x + 5, point4.y + 4)

                ctx.beginPath()
                ctx.moveTo(s.x, s.y)
                ctx.lineTo(p1.x, p1.y)

                if(plot.easing == 1) {
                    ctx.lineTo(p2.x, p2.y)
                    ctx.lineTo(p3.x, p3.y)
                    ctx.lineTo(p4.x, p4.y)
                } else if(plot.easing == 2) {
                    ctx.quadraticCurveTo(p2.x, p1.y, p2.x, p2.y)
                    ctx.lineTo(p3.x, p3.y)
                    ctx.quadraticCurveTo(p3.x, p4.y, p4.x, p4.y)
                } else if(plot.easing == 3) {
                    ctx.quadraticCurveTo(p1.x, p2.y, p2.x, p2.y)
                    ctx.lineTo(p3.x, p3.y)
                    ctx.quadraticCurveTo(p4.x, p3.y, p4.x, p4.y)
                }

                ctx.lineTo(e.x, e.y)
                ctx.stroke()
            }
        }
    }
}
