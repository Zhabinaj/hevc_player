import QtQuick 2.7
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.5
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.3

RowLayout{
    property RoundButton save_button: save //save_button - имя, через которое идет доступ
    property bool save_SEI: false

    RoundButton {
        id: save
        text: "Save"
        enabled: false
        palette.button: "#565656"
        palette.shadow: "#2d2d2d"
        palette.buttonText: "#d5cfcf"
        font.pointSize: 10
        radius: button_radius

        Layout.preferredWidth: 40
        Layout.preferredHeight: 25

        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter

        onClicked:{
            if (player_control.playing)
                player_control.playback_button.clicked();

            popup_save.open()
        }
    }

    Popup {
        id: popup_save

        background: Rectangle {
            x: (window.width/2)-300
            y: (window.height/2)-100
            width: 330
            height: 150
            radius: 15
            color: "black"
            opacity: 0.95

            GridLayout{
                anchors.fill: parent
                rows: 2
                columns: 3
                rowSpacing: 15 // пространство между строками
                columnSpacing: 15 // пространство между столбцами
                anchors.margins: 15

                Text{
                    id: popup_text
                    color: "#68011a" //цвет текста
                    text: "Save with SEI?"
                    font.pixelSize: 25

                    Layout.alignment: Qt.AlignHCenter
                    Layout.row: 0
                    Layout.column: 0
                    Layout.columnSpan: 3
                }

                RoundButton {
                    id: yes_button
                    text: "Yes"
                    font.pointSize: 10
                    enabled: true
                    palette.button: "#565656"
                    palette.shadow: "#2d2d2d"
                    palette.buttonText: "#d5cfcf"
                    radius: button_radius

                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 90

                    Layout.row: 1
                    Layout.column: 0

                    onClicked: {
                        save_SEI = true
                        fileDialogResultPath.open()
                        popup_save.close()
                    }
                }

                RoundButton {
                    id: no_button
                    text: "No"
                    font.pointSize: 10
                    enabled: true
                    palette.button: "#565656"
                    palette.shadow: "#2d2d2d"
                    palette.buttonText: "#d5cfcf"
                    radius: button_radius

                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 90

                    Layout.row: 1
                    Layout.column: 1

                    onClicked: {
                        save_SEI = false
                        fileDialogResultPath.open()
                        popup_save.close()
                    }
                }

                RoundButton {
                    id: cancel_button
                    text: "Cancel"
                    font.pointSize: 10
                    enabled: true
                    palette.button: "#565656"
                    palette.shadow: "#2d2d2d"
                    palette.buttonText: "#d5cfcf"
                    radius: button_radius

                    Layout.preferredHeight: 40
                    Layout.preferredWidth: 90

                    Layout.row: 1
                    Layout.column: 2

                    onClicked: { popup_save.close() }
                }
            }
        }
        closePolicy: Popup.NoAutoClose
    }

    FileDialog {
        id: fileDialogResultPath
        title: "Please choose a folder to save result sequence"
        folder: open_video.last_open_folder //открывает директорию последнего открытого файла
        selectMultiple: false
        selectFolder: true
        modality: Qt.ApplicationModal

        onAccepted:{
            var path = fileDialogResultPath.fileUrl
            session.saveThread(path, save_SEI) //сохранение в отдельный поток?
            fileDialogResultPath.close()
        }
        onRejected: fileDialogResultPath.close();
    }
}
