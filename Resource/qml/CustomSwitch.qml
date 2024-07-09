import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Switch {
    id: custom_switch

    enabled: true
    checked: false

    indicator: Rectangle {
        implicitWidth: 48
        implicitHeight: 26
        x: custom_switch.leftPadding
        y: parent.height / 2 - height / 2
        radius: width/2
        color: custom_switch.checked ? "#17a81a" : "black"
        border.width: 2
        border.color: custom_switch.checked ? "#17a81a" : "#cccccc"

        Rectangle {
            x: custom_switch.checked ? parent.width - width : 0
            width: 26
            height: 26
            radius: height/2
            color: custom_switch.down ? "#cccccc" : "#ffffff"
            border.color: custom_switch.checked ? (custom_switch.down ? "#17a81a" : "#21be2b") : "#999999"
        }
    }

    contentItem: Label{
        text: custom_switch.text
        font: custom_switch.font
        color: "#d5cfcf"
        verticalAlignment: Text.AlignVCenter
        leftPadding: custom_switch.indicator.width + custom_switch.spacing
    }
}
