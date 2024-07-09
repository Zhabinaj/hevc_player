import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.3

RowLayout {
    property FileDialog file_dialog_open: file_dialog_open
    property CustomButton open_file: open_file
    property string current_video: ""
    property string message_text: ""
    property url last_open_folder: ""

    Connections {
        target: session
        function onInitializationCompleted() {
            popup_init.close()
            init_done();

            //change Gui After Video Initialization:
            player_control.playback_button.enabled = true
            player_control.next_frame_button.enabled = true
            player_control.frame_slider.enabled = true;
            save_video.save_button.enabled = true;
            open_file.enabled = true;
        }
    }

    function init_done(){
        message_text = "Initialization done";
        popup_init.open()
    }

    function init_start(){
        message_text = "Initialization in progress";
        popup_init.open()
    }

    Popup {
        id: popup_init

        background: Rectangle {
            x: (window.width/2)-200
            y: (window.height/2)-50

            width: 400
            height: 100
            radius: 15
            color: "black"
            opacity: 0.95
            Text{
                anchors.centerIn: parent
                color: text_color
                text: message_text
                font.pixelSize: 25
            }          
        }
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    }

    RowLayout
    {
        spacing: 3
        Layout.preferredHeight: 50
        Layout.leftMargin: 5
        CustomButton {
            id: open_file
            text: "Open file"

            enabled: true
            Layout.fillHeight: true
            Layout.fillWidth: true

            onClicked: {                
                if (player_control.playing)
                    player_control.playback_button.clicked();

                if (!save_video.permission_to_open)
                    save_video.popup_wait.open();
                else{
                    if (save_video.saving)
                        save_video.abort_saving.open();
                    else
                        file_dialog_open.open()
                }
            }
        }
    }

    FileDialog {
        id: file_dialog_open
        title: "Open file"
        nameFilters: [ "Video files (*.hevc)" ]
        folder: shortcuts.home
        selectMultiple: false
        onAccepted: {
            if (open_video.current_video != ""){
                session.reset();
                video_flow.reset();
                player_control.reset();
                save_video.reset();
            }

            open_file.enabled = false;
            open_video.current_video = file_dialog_open.fileUrl;
            open_video.last_open_folder = file_dialog_open.folder;
            window.title = window.main_window_title + " - " + open_video.current_video

            init_start();

            session.initThread(open_video.current_video)
        }
        onRejected: {}
    }
}
