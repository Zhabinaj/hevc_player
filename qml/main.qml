import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

ApplicationWindow {
    property int button_radius: 15
    property string main_window_title: qsTr("Hevc viewer")

    id: window
    visible: true
    title: main_window_title
    visibility: "Maximized"
    color: "#363636" // угольный
    minimumWidth: 800
    minimumHeight: 600

    onClosing:{
        console.log("Button close was pressed")
        session.reset();
    }

    GridLayout {
        id: window_grid
        anchors.fill: parent
        columns: 3
        rows: 3
        anchors.margins: 6

        RowLayout
        {
            Layout.maximumHeight: 40
            FileBrowser {
                id: file_browser
                Layout.column: 0
                Layout.row: 0
            }

            PlayerControl {
                id: player_control
                Layout.column: 1
                Layout.row: 0
                Layout.columnSpan: 3
                Layout.rowSpan: 1
                }
        }

        VideoFlow {
            id: video_flow
            Layout.column: 0
            Layout.row: 2
            Layout.columnSpan: 3
        }
    }
}
