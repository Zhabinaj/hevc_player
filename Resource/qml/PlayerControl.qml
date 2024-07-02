import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

RowLayout {
    property CustomButton playback_button: playback_button
    property CustomButton prev_frame_button: prev_frame_button
    property CustomButton next_frame_button: next_frame_button
    property Slider frame_slider: frame_slider
    property Label total_frames_label: total_frames_label
    property Label current_frame_label: current_frame_label

    property string play: qsTr("▶")
    property string pause: qsTr("||")

    property bool playing: false //true когда петля работает

    Layout.leftMargin: 15

    Connections {
        target: session

        function onTotalFramesChanged(total_frames_count) {
            total_frames_label.changeLabelText(total_frames_count);
            frame_slider.changeToValue(total_frames_count);
        }

        function onCurrentFrameChanged(current_frame_) {
            // if slider is not holding then change label and slider info
            if (!frame_slider.pressed){
                // change slider and label of current frame
                current_frame_label.changeLabelText(current_frame_);
                frame_slider.value = current_frame_;
            }
            if (current_frame_<=1   )
                prev_frame_button.enabled = false;
            else if (player_control.playing == false)
                prev_frame_button.enabled = true;
            if (current_frame_ >=frame_slider.to)
                next_frame_button.enabled = false;
            else if (player_control.playing == false)
                next_frame_button.enabled = true;

        }

        function onVideoWasOver(){
            if (player_control.playing)
                player_control.playback_button.clicked();
		player_control.next_frame_button.enabled = false;
        }
    }

    function reset(){
        current_frame_label.setDefault();
        total_frames_label.setDefault();
        frame_slider.setDefault();
        prev_frame_button.enabled = false;
        next_frame_button.enabled = false;
        playback_button.enabled = false;
        frame_slider.enabled = false;
    }

    CustomButton {
        id: prev_frame_button
        text: qsTr("<")

        enabled: false
        Layout.fillHeight: true
        Layout.column: 1
        Layout.row: 0
        onClicked: {
            player_control.next_frame_button.enabled = true;
            session.prevFrameButtonClicked()
        }
    }

    CustomButton {
        id: playback_button
        text: play

        enabled: false
        Layout.fillHeight: true
        Layout.column: 2
        Layout.row: 0
        onClicked: {
            // pressed play button
            if (!player_control.playing)
            {
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
                // change player status of playing (is stopping)
                player_control.playing = false

                // Видео ставится на паузу, а на кнопке отображается команда "PLAY"
                player_control.playback_button.text = player_control.play

                // Переключать фреймы можно только когда видео стоит на паузе
                player_control.next_frame_button.enabled = true
                player_control.prev_frame_button.enabled = true

                session.pauseButtonClicked()
            }
        }
    }

    CustomButton {
        id: next_frame_button        
        text: ">"

        enabled: false
        Layout.fillHeight: true
        Layout.column: 3
        Layout.row: 0
        onClicked: { session.nextFrameButtonClicked(); }
    }

    Label {
        id: current_frame_label
        text: qsTr("Frame 0")

        property string current_frame: "Frame "
        color: text_color
        font.pointSize: 10
        Layout.preferredWidth: 110
        Layout.preferredHeight: 25

        Layout.alignment: Qt.AlignCenter
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        transformOrigin: Item.Center

        function setDefault() {
            text = qsTr("Frame 0");
        }

        function changeLabelText(frame_number) {
            text = qsTr("Frame " + frame_number);
        }        
    }

    Slider {
        id: frame_slider
        enabled: false
        font.weight: Font.Light
        from: 0.0
        to: 1.0
        stepSize: 1

        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter

        Layout.preferredHeight: 25 //высота расположения слайдера

        function setDefault() {
            frame_slider.value = 0.0;
        }

        function changeToValue(frame) {
            frame_slider.from = 1;
            frame_slider.to = frame;
        }
            // when user change current frame
        onMoved: {
            current_frame_label.changeLabelText(frame_slider.value);
        }

        onPressedChanged: {
            if (!frame_slider.pressed)
            {
                if (frame_slider.value>=frame_slider.to)
                    player_control.next_frame_button.enabled = false;
                if (frame_slider.value<=frame_slider.from)
                    player_control.prev_frame_button.enabled = false;
                session.changeFrameFromSlider(frame_slider.value)
            };
        }
    }

    Label {
        id: total_frames_label
        text: qsTr("0")

        color: text_color
        font.pointSize: 10
        Layout.preferredWidth: 80
        Layout.preferredHeight: 25

        Layout.alignment: Qt.AlignCenter
        verticalAlignment: Text.AlignVCenter

        function setDefault() {
            text = qsTr("0");
        }

        function changeLabelText(frames_count) {
            text = qsTr(""+frames_count);
        }
    }
}
