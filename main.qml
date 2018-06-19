import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3

ApplicationWindow {
    id:root
    title: qsTr("Hello World")
    visible: true
    width: 640
    height: 480

    menuBar: MenuBar {
           Menu {
               title: qsTr("File")
               MenuItem {
                   text: qsTr("&Open")
                   onTriggered: console.log("Open action triggered");
               }
               MenuItem {
                   text: qsTr("Exit")
                   onTriggered: Qt.quit();
               }
           }
       }

    toolBar: ToolBar {
           RowLayout {
               anchors.fill: parent
               ToolButton {
                   iconSource: "icons/plus.svg"
                   onClicked: {
                       var component = Qt.createComponent("device.qml")
                       var window = component.createObject(root)
                       window.show()
                   }
               }

           }
       }

    TabView {
        id: content
        anchors.fill: parent
        ListView {
            id: tracking
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
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
    }
}
