import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

ApplicationWindow {
    property string text_color: "#d5cfcf"
    property string main_window_title: qsTr("Hevc player")

    id: window
    visible: true
    title: main_window_title
    visibility: "Maximized"
    color: "#363636"
    minimumWidth: 1024
    minimumHeight: 768

    onClosing: session.reset()

    GridLayout {
        id: window_grid

        anchors.fill: parent
        columns: 4
        rows: 3
        anchors.margins: 10
        RowLayout
        {
            anchors.fill: parent //QML sends warnings in terminal, but without this resizing the main window does not work correctly
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

            ShowSEI{
                id: show_SEI
                Layout.column: 2
                Layout.row: 0
            }
        }

        VideoFlow {
            id: video_flow
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: 4
        }

        PlayerControl {
            id: player_control
            Layout.maximumHeight: 40
            Layout.column: 0
            Layout.columnSpan: 4
            Layout.row: 2
        }
    }
}
