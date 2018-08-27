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
import econ.camera.see3cam130 1.0

Text {
    id: statusText
    text: qsTr("Capture Image to start")
    color: "white"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    font.pointSize: 20
    SequentialAnimation on color {
        id: colorAnimate
        ColorAnimation { to: "orange"; duration: 1500 }
        ColorAnimation { to: "white"; duration: 800 }
        loops: Animation.Infinite
    }
}
