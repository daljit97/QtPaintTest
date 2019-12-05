import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import Drawing 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Flickable {
        id: scrollView
        anchors.fill: parent
        contentHeight: drawingCanvas.height
        DrawingCanvas {
            id: drawingCanvas
            width: parent.width
            height: 2000
            penColor: "red"
            onWidthChanged: drawingCanvas.initiateBuffer()
        }
    }

    MouseArea {
        anchors.fill: parent
        anchors.rightMargin: 20
        onPressed: drawingCanvas.penPressed(
                       Qt.point(mouseX, mouseY + scrollView.contentY))
        onPositionChanged: drawingCanvas.penMoved(
                               Qt.point(mouseX, mouseY + scrollView.contentY))
        onReleased: drawingCanvas.penReleased()
    }
    ToolBar {
        width: parent.width
        RowLayout {
            anchors.fill: parent
            CheckBox {
                id: oEraser
                text: "Outline Eraser"
            }
            Rectangle {
                color: "red"
                width: 20
                height: 20
                radius: width / 2
            }
        }
    }
}
