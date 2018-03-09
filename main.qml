import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.9
import com.ctdi.labelcreator.control 1.0

import econ.camera.property 1.0
import econ.camera.stream 1.0
import cameraenum 1.0

Window {
    id: mainWindow
    visible: true
    width: 1024
    height: 768
    title: qsTr("CTDI Label Creator")

    property int buttonMargin: 20
    property bool animatePrint : false

    Item {
        id: logo
        height: 40
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: mainWindow.buttonMargin
        Image {
            id: logoImage
            source: "qrc:/images/images/ctdiLogo.png"
            height: parent.height
            fillMode: Image.PreserveAspectFit
        }
    }

    Item {
        id: scanButtonContainer
        height: 40
        width: parent.width
        anchors.top: logo.bottom

        Button {
            id: scanButton
            height: parent.height
            width: 200
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                id: text
                text: qsTr("Capture Image")
                color: "blue"
                anchors.centerIn: parent
            }

            onPressed: {
                scanStatus.source = "qrc:/images/images/Running.gif"
                loadingAnimation.start()
                vidstreamproperty.makeShot("/home/curtis/Desktop", "jpg")
                control.captureImage()
            }
        }

        Image {
            id: scanStatus
            source: ""
            anchors.right: scanButton.right
            anchors.verticalCenter: parent.verticalCenter
            RotationAnimator {
                id: loadingAnimation
                target: scanStatus;
                from: 0;
                to: 360;
                duration: 2000;
                loops: Animation.Infinite
            }
        }

        Connections {
            target: vidstreamproperty
            onTitleTextChanged: {
                loadingAnimation.duration = 10
                loadingAnimation.loops = 1
                loadingAnimation.restart()
                scanStatus.source = "qrc:/images/images/pass.png"
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Grid {
        id: pictureActivityGrid
        width: parent.width
        height: parent.height - (mainWindow.buttonMargin * 2) - 20 - 40 - 40
        anchors.top: scanButtonContainer.bottom

        rows: 2
        columns: 2

        // Camera output
        Rectangle {
            id: cameraOutput
            height: parent.height / 2
            width: parent.width / 2
            border.color: "black"
            Text {
                text: qsTr("Loading Camera...")
                color: "black"
                anchors.centerIn: parent
                font.pointSize: 20
            }

            Videostreaming {
                id: vidstreamproperty
                Component.onCompleted: {
                    stopCapture()
                    closeDevice()
                    setDevice("/dev/video")
                    displayOutputFormat()
                    displayStillResolution()
                    displayVideoResolution()
                    displayEncoderList()
                    vidCapFormatChanged("0")
                    setResolution("640x480")
                    updateFrameInterval("YUYV (YUYV 4:2:2)", "640x480")
                    // Camera settings section
                    vidstreamproperty.changeSettings("9963776", "2")    //Brightness
                    vidstreamproperty.changeSettings("9963777", "5")    //Contrast
                    vidstreamproperty.changeSettings("9963778", "3")    //Saturation
                    vidstreamproperty.changeSettings("9963788", "1")    //White Balance Temperature, Auto
                    vidstreamproperty.changeSettings("9963802", "2")    //White Balance Temperature
                    vidstreamproperty.changeSettings("9963803", "1")    //Sharpness
                    vidstreamproperty.changeSettings("9963856", "0")    //Pan (Absolute)
                    vidstreamproperty.changeSettings("9963857", "0")    //Tilt (Absolute)
                    startAgain()
                    width = "640"
                    height = "480"
                    lastFPS("0")
                    masterModeEnabled()
                }
            }

        }
        // Last Picture Taken by Camera
        Rectangle {
            id: latestImageCaptured
            height: parent.height / 2
            width: parent.width / 2
            border.color: "black"
            color: "grey"
            Text {
                text: qsTr("Latest Picture Captured")
                color: "blue"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 20
            }

            Text {
                id: statusText
                text: qsTr("Capture Image to start")
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 20
                SequentialAnimation on color {
                    ColorAnimation { to: "orange"; duration: 1500 }
                    ColorAnimation { to: "white"; duration: 800 }
                    loops: Animation.Infinite
                }
                Connections {
                    target: vidstreamproperty
                    onTitleTextChanged: statusText.visible = false
                }
            }
            Image {
                id: scannedImage
                source: ""
                anchors.centerIn: parent
                height: parent.height / 2
                width: parent.width / 2
                fillMode: Image.PreserveAspectFit
            }

            Connections {
                target: vidstreamproperty
                onTitleTextChanged: scannedImage.source = "file:/home/curtis/Pictures/Tux.png"
            }
        }
        // Cropped Image
        Rectangle {
            id: croppedVersionLatestImage
            height: parent.height / 2
            width: parent.width / 2
            border.color: "black"
            color: "grey"
            Text {
                text: qsTr("Focused Information Region")
                color: "blue"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 20
            }

            Text {
                id: statusText2
                text: qsTr("Capture Image to start")
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 20
                SequentialAnimation on color {
                    ColorAnimation { to: "orange"; duration: 1500 }
                    ColorAnimation { to: "white"; duration: 800 }
                    loops: Animation.Infinite
                }
            }
        }
        // Text from Cropped image that is going to be used to print label
        Rectangle {
            id: infoToBePrinted
            height: parent.height / 2
            width: parent.width / 2
            border.color: "black"
            color: "grey"
            Text {
                text: qsTr("Information To Be Printed")
                color: "blue"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 20
            }

            Text {
                id: statusText3
                text: qsTr("Capture Image to start")
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 20
                SequentialAnimation on color {
                    ColorAnimation { to: "orange"; duration: 1500 }
                    ColorAnimation { to: "white"; duration: 800 }
                    loops: Animation.Infinite
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Item {
        id: printButtonContainer
        height: 40
        width: parent.width
        anchors.top: pictureActivityGrid.bottom
        anchors.bottom: parent.bottom
        Row {
            id: buttonRow
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20
            Button {
                id: printButton
                height: parent.height
                width: 140
                onPressed: {
                    printStatus.source = "qrc:/images/images/Running.gif"
                    animatePrint = true
                    control.printLabel()
                }
                Text {
                    id: printText
                    text: qsTr("Print label")
                    color: "blue"
                    anchors.centerIn: parent
                }
                Image {
                    id: printStatus
                    source: ""
                    anchors.right: printButton.right
                    anchors.verticalCenter: parent.verticalCenter
                    RotationAnimator {
                        target: printStatus;
                        from: 0;
                        to: 360;
                        duration: 2000;
                        loops: Animation.Infinite
                        running: animatePrint
                    }

                    Connections {
                        target: control
                        onPrintingComplete: { scanStatus.source = "qrc:/images/images/pass.png"; animatePrint = false }
                    }
                }
            }

            Button {
                id: resetButton
                height: parent.height
                width: 140
                onPressed: {
                    control.resetAllContent()
                    statusText.visible = true
                    scannedImage.source = ""
                    scanStatus.source = ""
                    printStatus.source = ""
                    animatePrint = false;
                    animateScan = false;
                }
                Text {
                    id: resetText
                    text: qsTr("Reset")
                    color: "red"
                    anchors.centerIn: parent
                }
            }
        }
    }
}
