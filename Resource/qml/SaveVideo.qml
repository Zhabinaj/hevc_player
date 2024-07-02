import QtQuick 2.7
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.3

RowLayout{
    property CustomButton save_button: save_button
    property Popup abort_saving: abort_saving
    property Popup popup_wait: popup_wait
    property bool save_SEI: false
    property bool saving: false    
    property bool permission_to_open: true
    property string save_label_text: ""

   Connections {
        target: session

        function onSavingProcessChanged(percent) {
            permission_to_open = true;
            save_button.enabled = true;
            if (percent == 100){
                save_label_text =  qsTr("Save completed");
                save.checked = false;
                saving = false;
            }
            else
               save_label_text =  qsTr(percent + "% saved")
        }
    }

    function reset(){
        save_label_text = ""
        save_button.enabled = false;
    }

    SeiToSaveWindow{
        id: sei_to_save_window
    }

    CustomButton {
        id: save_button
        text: "Save"

        enabled: false
        checkable: true
        Layout.fillHeight: true
        Layout.fillWidth: true
        onCheckedChanged: {
            if (checked){
                if (player_control.playing)
                    player_control.playback_button.clicked();
                sei_to_save_window.show();
            }
            else {
                if (saving)
                    session.stopSaving();                
            }
        }
    }

    Popup {
        id: popup_wait

        background: Rectangle {
            x: (window.width/2)-200
            y: (window.height/2)-50

            width: 400
            height: 100
            radius: 15
            color: "black"
            opacity: 0.95
            Text{
                anchors.centerIn: parent //выравнивание текста по центру
                color: text_color         //цвет текста
                text: "Please wait until the preparation \nfor saving is completed"
                font.pixelSize: 25
            }
        }
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    }

    Label {
        id: save_label
        text: save_label_text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        transformOrigin: Item.Center

        color: text_color

        Layout.column: 1
        Layout.row: 1
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: 80
        Layout.preferredHeight: 25
        font.pointSize: 10
    }

    Popup {
        id: abort_saving
        modal: true

        background: Rectangle {
            x: (window.width/2)-450
            y: (window.height/2)-90
            width: 330
            height: 150
            radius: 15
            color: "black"
            opacity: 0.95

            GridLayout{
                anchors.fill: parent
                rows: 2
                columns: 2
                rowSpacing: 15 // пространство между строками
                columnSpacing: 15 // пространство между столбцами
                anchors.margins: 15

                Text{
                    id: abort_saving_popup_text
                    color: text_color //цвет текста

                    text: "Do you want to complete \nsaving and open a new file?"
                    font.pixelSize: 20

                    Layout.alignment: Qt.AlignHCenter
                    Layout.row: 0
                    Layout.column: 0
                    Layout.columnSpan: 2
                }

                CustomButton {
                    id: abort_saving_yes_button
                    text: "Yes"

                    enabled: true
                    Layout.preferredWidth: 90
                    Layout.alignment: Qt.AlignHCenter
                    Layout.row: 1
                    Layout.column: 0
                    onClicked: {
                        save_button.checked = false;
                        abort_saving.close()
                        open_video.open_file.clicked();
                    }
                }

                CustomButton {
                    id: abort_saving_no_button
                    text: "No"

                    enabled: true
                    Layout.preferredWidth: 90
                    Layout.alignment: Qt.AlignHCenter
                    Layout.row: 1
                    Layout.column: 1
                    onClicked: {
                        abort_saving.close() }
                }
            }
        }
        closePolicy: Popup.NoAutoClose
    }

    FileDialog {
        id: fileDialogResultPath
        title: "Please choose a folder to save video"
        folder: open_video.last_open_folder //открывает директорию последнего открытого файла
        selectMultiple: false
        selectFolder: true
        modality: Qt.ApplicationModal

        onAccepted:{
            saving = true
            permission_to_open = false;
            save_button.enabled = false;
            save_label_text = "Preparing to save"
            var path = fileDialogResultPath.fileUrl
            session.saveThread(path, save_SEI)
            fileDialogResultPath.close()
        }
        onRejected: {
            save_button.checked = false;
            fileDialogResultPath.close();
        }
    }
}
