import QtQuick 2.0
import QtQuick.Controls 2.2

ApplicationWindow {
    id: root
    width: 100
    height: 100
    Item {
        ListView {
            id: bluetooth
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            delegate: Item {
                x: 5
                width: 80
                height: 40
                Row {
                    id: row1
                    spacing: 10
                    Rectangle {
                        width: 40
                        height: 40
                        color: colorCode
                    }

                    Text {
                        text: name
                        anchors.verticalCenter: parent.verticalCenter
                        font.bold: true
                    }
                }
            }
            model: ListModel {
                ListElement {
                    name: "Grey"
                    colorCode: "grey"
                }

                ListElement {
                    name: "Red"
                    colorCode: "red"
                }

                ListElement {
                    name: "Blue"
                    colorCode: "blue"
                }

                ListElement {
                    name: "Green"
                    colorCode: "green"
                }
            }
        }

        Row {
            id: row
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            anchors.top: bluetooth.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.topMargin: 0

            Button {
                id: button
                text: qsTr("Add")
            }
        }
    }
}
