import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3

RowLayout {

    property FileDialog file_dialog: file_dialog
    property string current_video: ""
    property int error: 0
    RowLayout
    {
        spacing: 3
        Layout.preferredHeight: 50
        Layout.leftMargin: 5
        RoundButton {
            id: open_file
            text: "Open file"

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
                //Если во время нажатия проигрывается видо, видео ставится на паузу. Просто для удобства
                if (player_control.playing)
                    session.pauseButtonClicked()

                file_dialog.open()
            }
        }

        RoundButton {
            id: save_as
            text: "Save as"
            enabled: false
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
                popup.open()
            }
        }

        Popup {
            id: popup
            contentItem: Text{
                text: "Function Save as is not available"
                width: 20
                height: 230
            }
            x: 700
            y: 500
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        }
    }

    FileDialog {
        id: file_dialog
        title: "Open file"
        nameFilters: [ "Video files (*.hevc)" ]
        folder: shortcuts.home
        selectMultiple: false

        onAccepted: {

            console.log("file_dialog.onAccepted.fileUrl: " + file_dialog.fileUrls)

            // Производим сброс бэка и фронта перед открытием нового видео
            if (file_browser.current_video != ""){
                session.reset();
                video_flow.reset();
            }

            //Получаем адрес ОДНОГО файла
            file_browser.current_video = file_dialog.fileUrl;

            console.log("initialize_video clicked()" + "; ")

            // показать выбранный файл в заголовке окна
            window.title = window.main_window_title + " - " + file_browser.current_video

            // Передача файла в Backend, пытаемся проинициализировать видео
            error = session.open(file_browser.current_video)
            if (error == 0){
                console.log("Video was initialized!");

                //change Gui After Video Initialization:
                player_control.playback_button.enabled = true
                //player_control.next_frame_button.enabled = true
                //player_control.prev_frame_button.enabled = true

                //Frame slider becomes available
                //player_control.frame_slider.enabled = true;
            }
            else {
                // если инициализация не удалась выводим код ошибки
                console.log("Video initialization was failed with error " + error);
            }
        }
        onRejected: {
            console.log("file_dialog.onRejected")
        }
    }
}
