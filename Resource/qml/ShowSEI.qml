import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

RowLayout{
    property CustomButton show_sei: show_sei

    CustomButton {
        id: show_sei
        text: "Show SEI"
        enabled: true

        Layout.fillHeight: true
        Layout.fillWidth: true
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

        flags: Qt.CustomizeWindowHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint

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

                //checked true - draw sei
                CustomSwitch{
                    id: switch_0
                    text:qsTr("Таймштамп")

                    Layout.column: 0
                    Layout.row: 0
                    onCheckedChanged: session.seiToShow(0,checked)
                }

                CustomSwitch{
                    id: switch_1
                    text:qsTr("Широта")

                    Layout.column: 0
                    Layout.row: 1
                    onCheckedChanged: session.seiToShow(1,checked)
                }

                CustomSwitch{
                    id: switch_2
                    text:qsTr("Долгота")

                    Layout.column: 0
                    Layout.row: 2
                    onCheckedChanged: session.seiToShow(2,checked)
                }

                CustomSwitch{
                    id: switch_3
                    text:qsTr("Высота")

                    Layout.column: 0
                    Layout.row: 3
                    onCheckedChanged: session.seiToShow(3,checked)
                }

                CustomSwitch{
                    id: switch_4
                    text:qsTr("Курс БЛА")

                    Layout.column: 0
                    Layout.row: 4
                    onCheckedChanged: session.seiToShow(4,checked)
                }

                CustomSwitch{
                    id: switch_5
                    text:qsTr("Курс OPS")

                    Layout.column: 0
                    Layout.row: 5
                    onCheckedChanged: session.seiToShow(5,checked)
                }

                CustomSwitch{
                    id: switch_6
                    text:qsTr("Тангаж БЛА")

                    Layout.column: 1
                    Layout.row: 0
                    onCheckedChanged: session.seiToShow(6,checked)
                }

                CustomSwitch{
                    id: switch_7
                    text:qsTr("Тангаж OPS")

                    Layout.column: 1
                    Layout.row: 1
                    onCheckedChanged: session.seiToShow(7,checked)
                }

                CustomSwitch{
                    id: switch_8
                    text:qsTr("Крен БЛА")

                    Layout.column: 1
                    Layout.row: 2
                    onCheckedChanged: session.seiToShow(8,checked)
                }

                CustomSwitch{
                    id: switch_9
                    text:qsTr("Поле зрения")

                    Layout.column: 1
                    Layout.row: 3
                    onCheckedChanged: session.seiToShow(9,checked)
                }

                CustomSwitch{
                    id: switch_10
                    text:qsTr("Дальность")

                    Layout.column: 1
                    Layout.row: 4
                    onCheckedChanged: session.seiToShow(10,checked)
                }

                CustomSwitch{
                    id: switch_11
                    text:qsTr("Сопровождение")

                    Layout.column: 1
                    Layout.row: 5
                    onCheckedChanged: session.seiToShow(11,checked)
                }                
            }
        }

        //QT Creator markered this as "Invalid property", but it works. Community Service QT says its ОК
        onClosing:{
            show_sei.enabled = true;
        }
    }
}
