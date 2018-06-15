import QtQuick 2.0
import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.9
import Qt.labs.platform 1.0
import com.ctdi.labelcreator.control 1.0
import QtGraphicalEffects 1.0

import econ.camera.property 1.0
import econ.camera.stream 1.0
import cameraenum 1.0
import econ.camera.see3cam81 1.0

Window {
    id: previewWindow
    width: 1920
    height: 1080
    title: "Camera output preview"

    MouseArea {
        anchors.fill: parent
        onPressed: { previewWindow.visible = false }
    }
}
