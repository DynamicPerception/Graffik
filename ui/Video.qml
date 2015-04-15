import QtQuick 2.3
import "qrc:///components/ui" as Components

Components.Box {
    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 10

        Grid {
            anchors.left: parent.left
            anchors.right: parent.right
            rowSpacing: 5
            columns: 5

            property int itemWidth: (width - 30) / 3
            Components.SpinBox {
                width: parent.itemWidth
                text: "1.5"
                validator: DoubleValidator {
                    notation: DoubleValidator.StandardNotation
                    locale: "C"
                }
            }

            Item { height: 30; width: 10 } //separator

            Components.LineEdit {
                width: parent.itemWidth
                text: "3"
                validator: IntValidator {}
            }

            Item {
                width: 20; height: 30
                Text {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -2
                    font.family: "Arial"
                    font.pixelSize: 16
                    color: "#818181"
                    text: "="
                }
            }

            Components.LineEdit {
                width: parent.itemWidth
                text: "4.5"
                validator: DoubleValidator {
                    notation: DoubleValidator.StandardNotation
                    locale: "C"
                }
            }

            Components.Label {
                horizontalAlignment: Text.AlignHCenter
                width: parent.itemWidth
                text: qsTr("Exposure")
            }

            Item { height: 1; width: 10 } //separator

            Components.Label {
                horizontalAlignment: Text.AlignHCenter
                width: parent.itemWidth
                text: qsTr("Buffer")
            }

            Item { height: 1; width: 20 } //separator

            Components.Label {
                horizontalAlignment: Text.AlignHCenter
                width: parent.itemWidth
                text: qsTr("Interval")
            }
        }

        Components.VerticalSeparator { width: parent.width }

        Components.Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 16
            text: qsTr("Shooting Duration")
        }

        Row {
            anchors.left: parent.left
            anchors.right: parent.right

            property int itemWidth: (width - 40) / 3
            Components.LabeledLineEdit {
                width: parent.itemWidth
                validator: IntValidator { top: 0 }
                text: "0"; label: "H"
            }

            Item {
                width: 20; height: 30
                Text {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -2
                    font.family: "Arial"
                    font.pixelSize: 16
                    color: "#818181"
                    text: ":"
                }
            }

            Components.LabeledLineEdit {
                width: parent.itemWidth
                validator: IntValidator { top: 59; bottom: 0 }
                text: "0"; label: "M"
            }

            Item {
                width: 20; height: 30
                Text {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -2
                    font.family: "Arial"
                    font.pixelSize: 16
                    color: "#818181"
                    text: ":"
                }
            }

            Components.LabeledLineEdit {
                width: parent.itemWidth
                validator: IntValidator { top: 59; bottom: 0 }
                text: "0"; label: "S"
            }
        }

        Components.VerticalSeparator { width: parent.width }

        Components.Label {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 16
            text: qsTr("Output")
        }

        Grid {
            anchors.left: parent.left
            anchors.right: parent.right
            columns: 3
            spacing: 10

            Components.LineEdit {
                width: 50
            }

            Components.LabeledLineEdit {
                width: parent.width - 120
                labelFontSize: 12
                label: "H:M:S"
            }

            Components.Dropdown {
                width: 50
            }
        }
    }
}
