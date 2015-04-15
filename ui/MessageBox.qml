import QtQuick 2.3

Item {
    id: form
    opacity: 0
    visible: opacity !== 0
    property string content: ""
    function hide() { hideAnimation.start() }
    function show() { showAnimation.start() }

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
        width: 350; height: okButton.y + okButton.height + 14
        color: "#242424"
        radius: 5

        Text {
            id: contentText
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 14
            y: 14
            font.family: fontRoman.name
            font.pixelSize: 14
            color: "#FFFFFF"
            wrapMode: Text.Wrap
            text: form.content
        }

        StandardButton {
            id: okButton
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: contentText.bottom
            anchors.topMargin: 14
            width: 100
            text: qsTr("OK")
            onClicked: form.hide()
        }
    }
}
