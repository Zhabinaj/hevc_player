import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

RoundButton {
    id: custom_button

    radius: 15
    palette.button: "#565656"
    palette.shadow: "#2d2d2d"
    palette.buttonText: "#d5cfcf"
    font.pointSize: 10

    Layout.preferredHeight: 40
    Layout.preferredWidth: 70

    Layout.alignment: Qt.AlignCenter
}
