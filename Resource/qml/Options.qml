import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

RowLayout{
    property RoundButton show_sei: show_sei

    RoundButton {
        id: show_sei
        text: "Show SEI"
        enabled: false
        checkable: true
        palette.button: "#565656"
        palette.shadow: "#2d2d2d"
        palette.buttonText: "#d5cfcf"

        radius: button_radius
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: 40
        Layout.preferredHeight: 25
        font.pointSize: 10

        onCheckedChanged: {
            CameraImage.showSei(checked) //checked true когда кнопку наживаем, false когда отжимаем
        }
    }
}
