import QtQuick 2.3

Item {
    id: lineEdit
    property alias label: labelText.text
    property alias text: input.text
    property alias validator: input.validator
    property int labelFontSize: 16
    property alias maxValue: input.maxValue
    property alias minValue: input.minValue
    property alias validationEnabled: input.validationEnabled
    signal accepted()
    clip: true
    height: 30

    LineEdit {
        id: input
        height: parent.height
        width: parent.width - labelImage.width + 4
        x: 0
        onAccepted: lineEdit.accepted()
    }

    BorderImage {
        id: labelImage
        height: parent.height
        width: labelText.implicitWidth + 16
        anchors.left: input.right
        anchors.leftMargin: -4
        border.bottom: 3
        border.left: 2
        border.right: 2
        border.top: 2
        source: "qrc:///images/ui/label.png"

        Text {
            id: labelText
            anchors.centerIn: parent
            font.family: "Arial"
            font.pixelSize: lineEdit.labelFontSize
            renderType: Text.NativeRendering
            color: "#000000"
        }
    }
}
