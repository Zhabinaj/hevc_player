import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

RowLayout {
    property RoundButton playback_button: playback_button
    property RoundButton prev_frame_button: prev_frame_button
    property RoundButton next_frame_button: next_frame_button
  //  property Slider frame_slider: frame_slider
   // property Label total_frames_label: total_frames_label
  //  property Label current_frame_label: current_frame_label

    property string play: qsTr("▶")
    property string pause: qsTr("||")

    property bool playing: false //true когда петля работает

    Layout.leftMargin: 15

    Connections {
        target: session
        onPause:{
            console.log("player_control.playback_button.onClicked(PAUSE)")

            // change player status of playing (is stopping)
            player_control.playing = false

            // Видео ставится на паузу, а на кнопке отображается команда "PLAY"
            player_control.playback_button.text = player_control.play

            // Переключать фреймы можно только когда видео стоит на паузе
            player_control.next_frame_button.enabled = true
            player_control.prev_frame_button.enabled = true
        }
    }

/*
    Connections {
        target: backend

        onCurrentFrameChanged: {
            // if slider is not holding then change label and slider info
            if (!frame_slider.pressed)
            {
                if (file_browser.current_video != "")
                    console.log(frame_number + " frame")

                // change slider and label of current frame
                current_frame_label.changeLabelText(frame_number);
                frame_slider.value = frame_number;
            }
        }

        onTotalFramesChanged: {
            total_frames_label.changeLabelText(total_frames_count);
            frame_slider.changeToValue(total_frames_count);
        }
    }
*/

    RoundButton {
        id: prev_frame_button
        text: qsTr("<")
        enabled: false

        palette.button: "#565656"
        palette.shadow: "#2d2d2d"
        palette.buttonText: "#d5cfcf"

        onClicked: {
            console.log("PlayerControl.prev_frame_button.onClicked()")
            //backend.prevFrameButtonClicked()
        }

        Layout.column: 1
        Layout.row: 0
        Layout.fillHeight: true
        radius: button_radius
        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: 50
        Layout.preferredHeight: 25
        font.pointSize: 10
    }

    RoundButton {
        id: playback_button
        text: play
        enabled: false

        palette.button: "#565656"
        palette.shadow: "#2d2d2d"
        palette.buttonText: "#d5cfcf"

        Layout.column: 2
        Layout.row: 0
        Layout.fillHeight: true
        radius: button_radius
        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: 50
        Layout.preferredHeight: 25
        font.pointSize: 10

        onClicked: {

            // pressed play button
            if (!player_control.playing)
            {
                console.log("player_control.playback_button.onClicked(PLAY)")

                // change player status of playing (is playing)
                player_control.playing = true

                // Видео воспроизводится, а на кнопке отображается команда "PAUSE"
                player_control.playback_button.text = player_control.pause

                // Когда видео воспроизводится, то нельзя переключать фреймы
                player_control.next_frame_button.enabled = false
                player_control.prev_frame_button.enabled = false

                // Запустить обработку кадров
                session.playButtonClicked()
            }
            // pressed pause button
            else
            {
                 session.pauseButtonClicked()
            }
        }
    }

    RoundButton {
        id: next_frame_button

        palette.button: "#565656"
        palette.shadow: "#2d2d2d"
        palette.buttonText: "#d5cfcf"

        text: ">"
        enabled: false

        onClicked: {
            console.log("PlayerControl.next_frame_button.onClicked()")

            //var frame_is_updated = backend.nextFrameButtonClicked();
        }

        Layout.column: 3
        Layout.row: 0
        Layout.fillHeight: true
        radius: button_radius
        Layout.alignment: Qt.AlignCenter
        Layout.preferredWidth: 50
        Layout.preferredHeight: 25
        font.pointSize: 10
    }
/*
    ColumnLayout {
        Layout.preferredHeight: 50
        Layout.leftMargin: 5
        RowLayout {
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            Label {
                id: current_frame_label
                text: qsTr(current_frame + zero)
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                transformOrigin: Item.Center

                color: "#d5cfcf"
                property string current_frame: "Frame "
                property string zero: "0"

                function setDefault() {
                    text = qsTr(current_frame + zero);
                }

                function changeLabelText(frame) {
                    text = qsTr(current_frame + frame);
                }

                Layout.column: 1
                Layout.row: 1
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 80
                Layout.preferredHeight: 25
                font.pointSize: 10

            }

            Label {
                id: total_frames_label
                text: qsTr(total_frames + zero)
                verticalAlignment: Text.AlignVCenter
                color: "#d5cfcf"

                property string total_frames: "from total "
                property string zero: "0"

                function setDefault() {
                    text = qsTr(total_frames + zero);
                }

                function changeLabelText(frames_count) {
                    text = qsTr(total_frames + frames_count);
                }

                Layout.column: 3
                Layout.row: 1
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 80
                Layout.preferredHeight: 25
                font.pointSize: 10
            }
        }

        Slider {
            id: frame_slider
            enabled: false
            font.weight: Font.Light
            from: 0.0
            to: 1.0

            stepSize: 1.0

            function setDefault() {
                frame_slider.value = 0.0;
            }

            function changeToValue(frame) {
                frame_slider.to = frame;
            }

            // when user change current frame
            onMoved: {
                current_frame_label.changeLabelText(frame_slider.value);
            }

            onPressedChanged: {
                if (!frame_slider.pressed)
                    var frame_is_updated = backend.updateFrame(frame_slider.value);
            }

            Layout.column: 2
            Layout.row: 1
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 25
        }
    }*/
}
