import QtQuick 2.7
import QtQuick.Window 2.3
import QtQuick.Layouts 1.12

Window{
    id: sei_to_save_window
    title: "Choose SEI for saving"
    visible: false
    minimumHeight: 500
    maximumHeight: 500
    minimumWidth: 450
    maximumWidth: 450

    flags: Qt.WindowTitleHint | Qt.CustomizeWindowHint

    Rectangle {
        width: parent.width
        height: parent.height
        color: "#363636"
        GridLayout{
            columns: 2
            rows: 8
            columnSpacing: 20
            rowSpacing: 20
            anchors.centerIn: parent

            CustomSwitch{
                id: switch_0
                text:qsTr("Таймштамп")

                Layout.column: 0
                Layout.row: 0
                onCheckedChanged: session.seiToSave(0,checked)
            }

            CustomSwitch{
                id: switch_1
                text:qsTr("Широта")

                Layout.column: 0
                Layout.row: 1
                onCheckedChanged: session.seiToSave(1,checked)
            }

            CustomSwitch{
                id: switch_2
                text:qsTr("Долгота")

                Layout.column: 0
                Layout.row: 2
                onCheckedChanged: session.seiToSave(2,checked)
            }

            CustomSwitch{
                id: switch_3
                text:qsTr("Высота")

                Layout.column: 0
                Layout.row: 3
                onCheckedChanged: session.seiToSave(3,checked)
            }

            CustomSwitch{
                id: switch_4
                text:qsTr("Курс БЛА")

                Layout.column: 0
                Layout.row: 4
                onCheckedChanged: session.seiToSave(4,checked)
            }

            CustomSwitch{
                id: switch_5
                text:qsTr("Курс OPS")

                Layout.column: 0
                Layout.row: 5
                onCheckedChanged: session.seiToSave(5,checked)
            }

            CustomSwitch{
                id: switch_6
                text:qsTr("Тангаж БЛА")

                Layout.column: 1
                Layout.row: 0
                onCheckedChanged: session.seiToSave(6,checked)
            }

            CustomSwitch{
                id: switch_7
                text:qsTr("Тангаж OPS")

                Layout.column: 1
                Layout.row: 1
                onCheckedChanged: session.seiToSave(7,checked)
            }

            CustomSwitch{
                id: switch_8
                text:qsTr("Крен БЛА")

                Layout.column: 1
                Layout.row: 2
                onCheckedChanged: session.seiToSave(8,checked)
            }

            CustomSwitch{
                id: switch_9
                text:qsTr("Поле зрения")

                Layout.column: 1
                Layout.row: 3
                onCheckedChanged: session.seiToSave(9,checked)
            }

            CustomSwitch{
                id: switch_10
                text:qsTr("Дальность")

                Layout.column: 1
                Layout.row: 4
                onCheckedChanged: session.seiToSave(10,checked)
            }

            CustomSwitch{
                id: switch_11
                text:qsTr("Сопровождение")

                Layout.column: 1
                Layout.row: 5
                onCheckedChanged: session.seiToSave(11,checked)
            }

            CustomButton {
                id: save_button_in_window
                text: "Save"

                enabled: true
                Layout.preferredWidth: 90
                Layout.alignment: Qt.AlignHCenter
                Layout.column: 0
                Layout.row: 6

                onClicked: {
                    sei_to_save_window.close()
                    file_dialog_save.open()
                }
            }

            CustomButton {
                id: cancel_button_in_window
                text: "Cancel"

                enabled: true
                Layout.preferredWidth: 90
                Layout.alignment: Qt.AlignHCenter
                Layout.row: 6
                Layout.column: 1
                onClicked: {
                    sei_to_save_window.close()
                    save_button.checked = false;                    
                }
            }
        }
    }
}
