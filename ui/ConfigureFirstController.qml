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

            property int buttonWidth: (width - 130 - 3 * columnSpacing) / 2
            Components.PortsDropdown {
                id: portDropdown
                width: 140
                dataModel: window.availablePorts
                z: 1
            }

            Components.StandardButton {
                id: assignButton
                width: parent.buttonWidth
                text: qsTr("Assign Controller 1")
                onClicked: {
                    if(portDropdown.text !== "") {
                        window.portName = portDropdown.text
                        assignButton.enabled = false
                        cancelButton.enabled = false
                        window.assignAddressRequest(portDropdown.text, 3)
                    }
                }
            }

            Components.StandardButton {
                id: cancelButton
                width: parent.buttonWidth
                text: qsTr("Cancel Configuration")
                onClicked: {
                    form.hide()
                    connectionManager.show()
                }
            }

            Components.Label {
                width: portDropdown.width
                text: "Port"
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

            text: qsTr("Make sure that controllers are not daisychained and are not powered via DC jack. Attach one controller via USB, select it's COM port, then assgn the controller. Mark your controllers with numbers for later reference.")
        }
    }
}
