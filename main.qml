import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.9
import com.ctdi.labelcreator.control 1.0

import econ.camera.property 1.0
import econ.camera.stream 1.0
import cameraenum 1.0
import econ.camera.see3cam81 1.0

Window {
    id: mainWindow
    visible: true
    width: Screen.width
    height: Screen.height
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

    Row {
        id: buttonRow
        height: 40
        anchors.top: logo.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 5

        Item {
            height: parent.height
            width: 200
        Button {
            id: captureButton
            anchors.fill: parent
            onPressed: {
                captureStatus.source = "qrc:/images/images/Running.gif"
                loadingAnimation.start()
                vidstreamproperty.makeShot("/home/curtis/Desktop", "jpg")
                control.captureImage()
            }
            Text {
                id: captureText
                text: qsTr("Capture Image")
                color: "blue"
                anchors.centerIn: parent
            }

            Image {
                id: captureStatus
                source: ""
                anchors.right: captureButton.right
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                RotationAnimator {
                    id: loadingAnimation
                    target: captureStatus;
                    from: 0;
                    to: 360;
                    duration: 2000;
                    loops: Animation.Infinite
                }

                Connections {
                    target: vidstreamproperty
                    onTitleTextChanged: {
                        loadingAnimation.duration = 10
                        loadingAnimation.loops = 1
                        loadingAnimation.restart()
                        captureStatus.source = "qrc:/images/images/pass.png"
                    }
                }
            }
        }
        }

        Button {
            id: printButton
            height: parent.height
            width: 200
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
                anchors.rightMargin: 5
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
                    onPrintingComplete: {
                        scanStatus.source = "qrc:/images/images/pass.png"
                        animatePrint = false
                    }
                }
            }
        }

        Button {
            id: resetButton
            height: parent.height
            width: 200
            onPressed: {
                control.resetAllContent()
                statusText.visible = true
                scannedImage.source = ""
                captureStatus.source = ""
                printStatus.source = ""
                animatePrint = false;
            }
            Text {
                id: resetText
                text: qsTr("Reset")
                color: "red"
                anchors.centerIn: parent
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Grid {
        id: pictureActivityGrid
        width: parent.width
        height: parent.height - (mainWindow.buttonMargin * 2) - 20 - 40 - 10

        anchors.top: buttonRow.bottom
        anchors.topMargin: 20

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
                anchors.centerIn: parent
                See3Cam81 {
                    id: see3Cam81
                    Component.onCompleted: {
                        //setEffectMode(See3Cam81.EFFECT_GRAYSCALE)
                        setEffectMode(See3Cam81.EFFECT_NORMAL)
                        setFocusMode(See3Cam81.CONTINUOUS_FOCUS_81)
                    }
                }

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
                    height = "460"
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
}
