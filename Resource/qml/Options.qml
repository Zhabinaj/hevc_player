import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

RowLayout{
    property RoundButton show_sei: show_sei

    RoundButton {
        id: show_sei
        text: "Show SEI"
        enabled: true
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

        onClicked: {
            sei_window.show();
            show_sei.enabled = false;
        }
    }

    Window{
        id: sei_window
        title: "Show SEI"
        visible: false
        minimumHeight: 500
        maximumHeight: 500
        minimumWidth: 450
        maximumWidth: 450

        flags: "WindowCloseButtonHint";

        Rectangle {
            width: parent.width
            height: parent.height
            color: "#363636"
            GridLayout{
                columns: 2
                rows: 7
                columnSpacing: 20
                rowSpacing: 20
                anchors.centerIn: parent

                //если выбран свич checked true - рисуем сеи, нажали и стало false - убрали
                CustomSwitch{
                    id: switch_0
                    text:qsTr("Таймштамп")
                    Layout.column: 0
                    Layout.row: 0
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(0,checked)
                    }
                }

                CustomSwitch{
                    id: switch_1
                    text:qsTr("Широта")
                    Layout.column: 0
                    Layout.row: 1
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(1,checked)
                    }
                }

                CustomSwitch{
                    id: switch_2
                    text:qsTr("Долгота")
                    Layout.column: 0
                    Layout.row: 2
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(2,checked)
                    }
                }

                CustomSwitch{
                    id: switch_3
                    text:qsTr("Высота")
                    Layout.column: 0
                    Layout.row: 3
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(3,checked)
                    }
                }

                CustomSwitch{
                    id: switch_4
                    text:qsTr("Курс БЛА")
                    Layout.column: 0
                    Layout.row: 4
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(4,checked)
                    }
                }

                CustomSwitch{
                    id: switch_5
                    text:qsTr("Курс OPS")
                    Layout.column: 0
                    Layout.row: 5
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                       session.showSei2(5,checked)
                    }
                }

                CustomSwitch{
                    id: switch_6
                    text:qsTr("Тангаж БЛА")
                    Layout.column: 1
                    Layout.row: 0
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(6,checked)
                    }
                }

                CustomSwitch{
                    id: switch_7
                    text:qsTr("Тангаж OPS")
                    Layout.column: 1
                    Layout.row: 1
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(7,checked)
                    }
                }

                CustomSwitch{
                    id: switch_8
                    text:qsTr("Крен БЛА")
                    Layout.column: 1
                    Layout.row: 2
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(8,checked)
                    }
                }

                CustomSwitch{
                    id: switch_9
                    text:qsTr("Поле зрения")
                    Layout.column: 1
                    Layout.row: 3
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(9,checked)
                    }
                }

                CustomSwitch{
                    id: switch_10
                    text:qsTr("Дальность")
                    Layout.column: 1
                    Layout.row: 4
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(10,checked)
                    }
                }

                CustomSwitch{
                    id: switch_11
                    text:qsTr("Сопровождение")
                    Layout.column: 1
                    Layout.row: 5
                    enabled: true
                    checked: false

                    onCheckedChanged: {
                        session.showSei2(11,checked)
                    }
                }                
            }
        }

        //QT ругается, но это работает. Community Service QT говорит это ОК
        onClosing:{
            show_sei.enabled = true;
        }
    }
}
