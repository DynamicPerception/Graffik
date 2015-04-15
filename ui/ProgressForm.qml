import QtQuick 2.3

Item {
    id: progressForm

    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: 0.75
    }

    Text {
        id: progressText
        anchors.bottom: progressColumn.top
        anchors.bottomMargin: 10
        font.family: fontRoman.name
        font.pixelSize: 15
        renderType: Text.NativeRendering
        color: "#FFFFFF"
        text: window.programProgress + "%"
        visible: window.motorsOnStartPositions
        x: {
            var tmp = progressRect.width > 320 ? 320 : progressRect.width
            return progressColumn.x + tmp - implicitWidth / 2
        }
    }

    Column {
        id: progressColumn
        anchors.centerIn: parent
        width: 320
        spacing: 20

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            height: 4
            color: "#FFFFFF"
            clip: true
            visible: window.motorsOnStartPositions

            Rectangle {
                id: progressRect
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: parent.width * window.programProgress / 100.0
                color: "#276DC5"

                Behavior on width {
                    NumberAnimation { duration: 100; easing.type: Easing.InQuad }
                }
            }
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            font.family: fontRoman.name
            font.pixelSize: 15
            renderType: Text.NativeRendering
            visible: window.motorsOnStartPositions
            text: qsTr("<font color='#7F7F7F'>Movement procedure is performed, please use the " +
                       "buttons on the top bar to </font><font color='#FFFFFF'>stop</font><font " +
                       "color='#7F7F7F'> or </font><font color='#FFFFFF'>pause</font>")
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            font.family: fontRoman.name
            font.pixelSize: 15
            renderType: Text.NativeRendering
            text: qsTr("Sending motors to start positions, please wait...")
            color: "#FFFFFF"
            visible: !window.motorsOnStartPositions
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {}
    }
}
