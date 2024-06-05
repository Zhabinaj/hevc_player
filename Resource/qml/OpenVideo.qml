import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.3

RowLayout {

    property FileDialog file_dialog: file_dialog
    property string current_video: ""
    property int error: 0
    property string message_text: ""
    //property url def : shortcuts.home
    property url last_open_folder: ""


    function init_done(){
        message_text = "Initialization done";
        popup_test.open()
    }

    function init_start(){
        message_text = "Initialization in progress";
        popup_test.open()
    }

    function save_clicked(){
        message_text = "Function Save as is not available";
        popup_test.open()
    }

    //======= error - сработавшая ошибка инициализации
    //правильно словить в бэкенде, передать сообщение сюда и правильно отработать
    function init_error(error){
        message_text = qsTr("Video initialization was failed with error: " + error)
        popup_test.open()
    }

    Popup {
        id: popup_test

        background: Rectangle {
            x: (window.width/2)-200
            y: (window.height/2)-50

            width: 400
            height: 100
            radius: 15
            color: "black" //"#240008"//
            // border.color: "red"
            opacity: 0.95
            Text{
                anchors.centerIn: parent //выравнивание текста по центру
                color: "#68011a" //"#B8B8B8"// //цвет текста
                text: message_text
                font.pixelSize: 25
            }          
        }
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    }

    Connections {
        target: session
        onInitializationCompleted: {
            popup_test.close()
            init_done();

            //change Gui After Video Initialization:
            player_control.playback_button.enabled = true
            player_control.next_frame_button.enabled = true

            //Frame slider becomes available
            player_control.frame_slider.enabled = true;

            options.show_sei.enabled = true;
            save_video.save_button.enabled = true;

            //show_sei.enabled = true;
        }
    }

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
                    player_control.playback_button.clicked();

                file_dialog.open()
            }
        }
    }

    FileDialog {
        id: file_dialog
        title: "Open file"
        nameFilters: [ "Video files (*.hevc)" ]
        folder: shortcuts.home
        selectMultiple: false
        onAccepted: {
             /* раскомментировать для отладки
            console.log("file_dialog.onAccepted.fileUrl: " + file_dialog.fileUrls)
            */

            // Производим сброс бэка и фронта перед открытием нового видео
            if (open_video.current_video != ""){
                session.reset();
                video_flow.reset();
                player_control.reset();
                save_video.save_button.enabled = false;
                options.show_sei.checked = false;
                if (options.show_sei.enabled == true)
                    options.show_sei.enabled = false;

            }

            //Получаем адрес ОДНОГО файла
            open_video.current_video = file_dialog.fileUrl;
            open_video.last_open_folder = file_dialog.folder;


             /* раскомментировать для отладки
            console.log("initialize_video clicked()" + "; ")
            */

            // показать выбранный файл в заголовке окна
            window.title = window.main_window_title + " - " + open_video.current_video

            init_start();

            // Передача файла в Backend, пытаемся проинициализировать видео в отдельном потоке
            session.initThread(open_video.current_video)

        }

        onRejected: {
             /* раскомментировать для отладки
            console.log("file_dialog.onRejected")
            */
        }
    }
}

/*
        Button
        {
            id: buttonOpenResultPath

            text: qsTr("PATH")
            autoExclusive: false
            highlighted: false
            Layout.preferredWidth: 75
            Layout.preferredHeight: 40
            font.pointSize: 8

            onClicked: fileDialogResultPath.open()
                FileDialog
                {
                    id: fileDialogResultPath
                    title: "Please choose a folder to save result sequence"
                    folder: shortcuts.home
                    selectMultiple: false
                    selectFolder: true
                    modality: Qt.ApplicationModal

                    onAccepted:
                    {
                        var path = fileDialogResultPath.fileUrl

                        //session.ResultPath = path
                        session.saveVideo(path, save_SEI) //сохранение в отдельный поток?

                        fileDialogResultPath.close()

                    }
                    onRejected:
                    {
                        fileDialogResultPath.close();
                    }
                }
        }



  */
