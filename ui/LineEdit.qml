import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

TextField {
    id: textField
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter
    onFocusChanged: if(focus) window.currentItem = textField

    property real maxValue: 0
    property real minValue: 0
    property bool validationEnabled: false

    onTextChanged: {
        if(!validationEnabled)
            return

        var current = parseFloat(text)
        if(text.length === 0)
            text = minValue
        else if(current > maxValue)
            text = maxValue
        else if(current < minValue)
            text = minValue
    }

    function hide() {
        focus = false
        textField.accepted()
    }

    function containsPoint(px, py) {
        if(px < 0) return false
        if(px > width) return false
        if(py < 0) return false
        if(py > height) return false
        return true
    }

    style: TextFieldStyle {
        textColor: "#AAAAAA"
        font.family: fontLight.name
        font.pixelSize: 18
        renderType: Text.NativeRendering

        background: Rectangle {
            implicitHeight: 30
            radius: 4
            color: "#2B2B2B"

            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: 1
                radius: 4
                color: "#161616"
            }
        }
    }
}
