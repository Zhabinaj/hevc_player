import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Switch {
    id: control
    indicator: Rectangle {
        implicitWidth: 48
        implicitHeight: 26
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: width/2
        color: control.checked ? "#17a81a" : "#2d2d2d" //"#17a81a" : "#ffffff"
        border.width: 2 //youtube
        border.color: control.checked ? "#17a81a" : "#cccccc"

        Rectangle {
            x: control.checked ? parent.width - width : 0
            width: 26
            height: 26
            radius: height/2
            color: control.down ? "#cccccc" : "#ffffff"
            border.color: control.checked ? (control.down ? "#17a81a" : "#21be2b") : "#999999"
        }
    }

    contentItem: Label{
        text: control.text
        font: control.font
        color: "#d5cfcf"
        verticalAlignment: Text.AlignVCenter
        leftPadding: control.indicator.width + control.spacing
    }
}
