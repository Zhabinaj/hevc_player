import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

ApplicationWindow {
    property int button_radius: 15
    property string main_window_title: qsTr("Hevc player")

    id: window

    visible: true
    title: main_window_title
    visibility: "Maximized"
    color: "#363636"
    minimumWidth: 1024
    minimumHeight: 768


    onClosing:{
        console.log("Hevc player is closed")
        session.reset();
    }

    GridLayout {
        id: window_grid
        anchors.fill: parent
        columns: 4
        rows: 3
        anchors.margins: 6
        RowLayout
        {
            anchors.fill: parent
            Layout.maximumHeight: 40
            OpenVideo {
                id: open_video
                Layout.column: 0
                Layout.row: 0                
            }

            SaveVideo{
                id: save_video
                Layout.column: 1
                Layout.row: 0
            }

            Options{
                id: options
                Layout.column: 2
                Layout.row: 0
            }

            PlayerControl {
                id: player_control
                Layout.column: 3
                Layout.row: 0
            }
        }

        VideoFlow {
            id: video_flow
            Layout.column: 0
            Layout.row: 2
            Layout.columnSpan: 4
        }
    }
}
