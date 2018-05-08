import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.9
import Qt.labs.platform 1.0
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
            RoundButton {
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
                    text: qsTr("Capture image")
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
                        onCaptureSuccess: {
                            loadingAnimation.duration = 10
                            loadingAnimation.loops = 1
                            loadingAnimation.restart()
                            captureStatus.source = "qrc:/images/images/pass.png"
                        }
                    }
                }
            }
        }

        RoundButton {
            id: printButton
            height: parent.height
            width: 200
            onPressed: {
                printStatus.source = "qrc:/images/images/Running.gif"
                printingAnimation.start()
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
                    id: printingAnimation
                    target: printStatus;
                    from: 0;
                    to: 360;
                    duration: 2000;
                    loops: Animation.Infinite
                }

                Connections {
                    target: control
                    onPrintingComplete: {
                        printingAnimation.duration = 10
                        printingAnimation.loops = 1
                        printingAnimation.restart()
                        printStatus.source = "qrc:/images/images/pass.png"
                    }
                    onPrintingError: {
                        printingAnimation.duration = 10
                        printingAnimation.loops = 1
                        printingAnimation.restart()
                        printStatus.source = "qrc:/images/images/fail.png"
                    }
                }
            }
        }

        RoundButton {
            id: resetButton
            height: parent.height
            width: 200
            onPressed: {
                control.resetAllContent()
                cameraText.color = "black"
                cameraText.text = "Loading Camera..."
                statusText.visible = true
                statusText.color = "white"
                statusText.text = "Capture Image to start"
                scannedImage.source = ""
                captureStatus.source = ""
                loadingAnimation.duration = 2000
                loadingAnimation.loops = Animation.Infinite
                printStatus.source = ""
                printingAnimation.duration = 2000
                printingAnimation.loops = Animation.Infinite
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
                id: cameraText
                text: qsTr("Loading Camera...")
                color: "black"
                anchors.centerIn: parent
                font.pointSize: 20

                Connections {
                    target: vidstreamproperty
                    onUnableToOpenCam: {
                        cameraText.text = "Error opening Camera"
                        cameraText.color = "red"
                    }
                }
            }

            Item {
                id: takePicture
                height: 60
                width: height
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.left: parent.left
                anchors.leftMargin: 10
                Image {
                    id: captureImage
                    source: "qrc:/images/images/takePicture.png"
                    sourceSize.height: parent.height
                    sourceSize.width: parent.width
                }
                MouseArea {
                    id: takePictureBtnArea
                    anchors.fill: parent
                    hoverEnabled: true

                    ToolTip {
                        parent: takePictureBtnArea
                        visible: takePictureBtnArea.containsMouse
                        text:"Capture image"
                    }
                    onPressed: {
                        captureStatus2.source = "qrc:/images/images/Running.gif"
                        loadingAnimation2.start()
                        vidstreamproperty.makeShot("/home/curtis/Desktop", "jpg")
                        control.captureImage()
                    }
                }
            }

            Image {
                id: captureStatus2
                source: ""
                height: 50
                width: height
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.left: takePicture.right
                anchors.leftMargin: 10
                RotationAnimator {
                    id: loadingAnimation2
                    target: captureStatus2;
                    from: 0;
                    to: 360;
                    duration: 2000;
                    loops: Animation.Infinite
                }

                Connections {
                    target: vidstreamproperty
                    onCaptureSuccess: {
                        loadingAnimation2.duration = 10
                        loadingAnimation2.loops = 1
                        loadingAnimation2.restart()
                        captureStatus2.source = "qrc:/images/images/pass.png"
                    }
                }
            }

            Item {
                id: resetCamera
                height: 60
                width: height
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.right: parent.right
                anchors.rightMargin: 10
                Image {
                    id: resetImage
                    source: "qrc:/images/images/resetCamera.png"
                    sourceSize.height: parent.height
                    sourceSize.width: parent.width
                }
                MouseArea {
                    id: resetCameraBtnArea
                    anchors.fill: parent
                    hoverEnabled: true

                    ToolTip {
                        parent: resetCameraBtnArea
                        visible: resetCameraBtnArea.containsMouse
                        text:"Refresh Camera Preview"
                    }
                    onPressed: {
                        console.log("RESET CAMERA pressed")
                        vidstreamproperty.startCam()
                    }
                }
            }

            Videostreaming {
                id: vidstreamproperty
                anchors.centerIn: parent
                See3Cam81 {
                    id: see3Cam81
                    Component.onCompleted: {
                        setEffectMode(See3Cam81.EFFECT_NORMAL)
                        setFocusMode(See3Cam81.CONTINUOUS_FOCUS_81)
                    }
                }

                Component.onCompleted: { startCam() }
                Timer {
                    id: timer
                    interval: 5000; repeat: false
                    onTriggered: { control.setFocus() }
                }

                function startCam() {
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
                    vidstreamproperty.changeSettings("9963776", "2")    // Brightness
                    vidstreamproperty.changeSettings("9963777", "5")    // Contrast
                    vidstreamproperty.changeSettings("9963778", "3")    // Saturation
                    vidstreamproperty.changeSettings("9963788", "0")    // White Balance Temperature, Auto
                    vidstreamproperty.changeSettings("9963802", "2")    // White Balance Temperature
                    vidstreamproperty.changeSettings("9963803", "1")    // Sharpness
                    vidstreamproperty.changeSettings("10094856", "0")    // Pan (Absolute)
                    vidstreamproperty.startAgain()
                    width = "640"
                    height = "460"
                    lastFPS("0")
                    masterModeEnabled()
                    timer.start()
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
                    onCaptureSuccess: statusText.visible = false
                    onCaptureFail: { statusText.color = "red"; statusText.text = "FAILED TO CAPTURE IMAGE" }
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
                onCaptureSuccess: {
                    scannedImage.source = StandardPaths.standardLocations(StandardPaths.PicturesLocation) + "/latestCapture.jpg"
                }
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

