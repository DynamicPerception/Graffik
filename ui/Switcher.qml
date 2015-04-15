import QtQuick 2.3

Item {
    id: button
    signal clicked()
    property bool checked: false
    height: 20; width: 59

    Image {
        id: background
        source: "qrc:///images/ui/switcherBg.png"
    }

    Image {
        id: handler
        source: "qrc:///images/ui/switcherHandler.png"
        x: button.checked ? 0 : button.width - width + 1
        y: 0

        Behavior on x {
            NumberAnimation { duration: 50; easing.type: Easing.InQuad }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            button.checked = !button.checked
            button.clicked()
        }
    }
}
