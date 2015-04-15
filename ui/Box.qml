import QtQuick 2.3
import QtGraphicalEffects 1.0

Item {
    id: box
    property alias color: background.color
    width: 295

    RectangularGlow {
        anchors.fill: parent
        glowRadius: 7; spread: 0
        color: "#67ADF7"
        cornerRadius: 12
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#242424"
        radius: 5
    }
}
