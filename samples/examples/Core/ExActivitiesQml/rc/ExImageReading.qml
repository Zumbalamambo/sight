import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.3

import styleQml 1.0
import uiActivitiesQml 1.0
import fwVTKQml 1.0
import ExActivitiesQml 1.0

Activity {
    id: exImageDisplaying

    // App manager associated to this activity
    appManager: ImageReadingManager {
        id: appManager
        frameBuffer: scene3D
    }

    ColumnLayout {

        anchors.fill: parent

        ToolBar {

            RowLayout {

                ToolButton{
                    text: qsTr("open")
                    onClicked: appManager.openImage()
                }
            }
        }

        Rectangle {
            id: sceneRec
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#006699"

            // Frame buffer to display the 3D scene
            FrameBufferItem {
                id: scene3D
                anchors.fill: parent

                onReady: appManager.createVtkScene()
            }
        }
    }
}
