import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import Drawing 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    property int pageWidth: 2000
    property int pageHeight: 2000

    Flickable {
        id: scrollView
        width: pageWidth
        height: pageHeight
        contentHeight: drawingCanvas.height
        DrawingCanvas {
            id: drawingCanvas
            width: parent.width
            height: pageHeight
            penColor: "black"
            penWidth: 20
            onWidthChanged: drawingCanvas.initiateBuffer()
            transformOrigin: Item.TopLeft
            window: appWindow
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
    //    ToolBar {
    //        width: parent.width
    //        RowLayout {
    //            anchors.fill: parent
    //            CheckBox {
    //                id: oEraser
    //                text: "Outline Eraser"
    //            }
    //            Button {
    //                text: "Save SVG"
    //                onClicked: drawingCanvas.saveSvg()
    //            }
    //            Slider {
    //                id: scaleSlider
    //                from: 1
    //                to: 3
    //                onValueChanged: {
    //                    drawingCanvas.scale = value
    //                }
    //            }
    //        }
    //    }
}
