import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15

GridLayout {

    property Image window_image: cameraImageVideo
    property string img: "image://camera/hevcTV"
    property string default_image: Qt.resolvedUrl("../Images/default.png")

    Layout.leftMargin: 10
    Layout.rightMargin: 10
    Layout.topMargin: 10
    Layout.bottomMargin: 10

    Connections {
        target: CameraImage
        onImageChangedHevcTV: cameraImageVideo.reload();
    }

    function reset(){
        cameraImageVideo.source = "";
        cameraImageVideo.source = default_image;
    }

    Rectangle {
        id: image_rect
        Layout.column: 0
        Layout.row: 0
        Layout.fillHeight: true
        Layout.fillWidth: true
        width: 800
        height: 600
        color: "black"
        Image {
            id: cameraImageVideo
            anchors.fill: parent
            source: default_image
            function reload() {
                source = ""
                source = img
            }
        }
    }
}
