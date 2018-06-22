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
    id: mainWindow
    visible: true
    width: Screen.width
    height: Screen.height
    title: qsTr("CTDI Label Creator")

    property int buttonMargin: 20
    property bool readyToPrint : false

    CameraPreview {
        id: preview
        visible: false

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
                if (control.cameraDiscovery()) {
                    stopCapture()
                    closeDevice()
                    setDevice("/dev/video")
                    displayOutputFormat()
                    displayStillResolution()
                    displayVideoResolution()
                    displayEncoderList()
                    vidCapFormatChanged("0")
                    setResolution("1920x1080")
                    updateFrameInterval("YUYV (YUYV 4:2:2)", "1920x1080")
                    // Camera settings section
                    vidstreamproperty.changeSettings("9963776", "2")    // Brightness
                    vidstreamproperty.changeSettings("9963777", "5")    // Contrast
                    vidstreamproperty.changeSettings("9963778", "3")    // Saturation
                    vidstreamproperty.changeSettings("9963788", "0")    // White Balance Temperature, Auto
                    vidstreamproperty.changeSettings("9963802", "2")    // White Balance Temperature
                    vidstreamproperty.changeSettings("9963803", "1")    // Sharpness
                    vidstreamproperty.changeSettings("10094856", "0")    // Pan (Absolute)
                    vidstreamproperty.startAgain()
                    width = "1920"
                    height = "1080"
                    lastFPS("0")
                    masterModeEnabled()
                    timer.start()
                } else {
                    // Emit to tell GUI to display error message
                    unableToOpenCam()
                }
            }
        }
    }

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
        id: resetButton
        height: 60
        width: height
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: mainWindow.buttonMargin
        Image {
            id: resetBtnImage
            source: "qrc:/images/images/resetCamera.png"
            height: parent.height
            fillMode: Image.PreserveAspectFit
            ColorOverlay {
                anchors.fill: resetBtnImage
                source: resetBtnImage
                color: "#ff0000"  // make image red
            }
            MouseArea {
                id: resetBtnArea
                anchors.fill: parent
                hoverEnabled: true

                ToolTip {
                    parent: resetBtnArea
                    visible: resetBtnArea.containsMouse
                    text:"Reset Capture"
                }
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
                    printingAnimation.stop()
                    imageColour.color = "white"
                    readyToPrint = false
                }
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Grid {
        id: pictureActivityGrid
        width: parent.width
        height: parent.height - (mainWindow.buttonMargin * 2) - 40

        anchors.top: logo.bottom
        anchors.topMargin: 20

        rows: 2
        columns: 2
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // Section 1: Camera output
        Rectangle {
            id: cameraOutput
            height: parent.height / 2
            width: parent.width / 2
            border.color: "black"

            Rectangle {
                id: spacer
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 10
                height: 20
                y: parent.height * 0.01
            }

            Row {
                id: buttonRow
                anchors.top: spacer.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10
                Item {
                    id: takePicture
                    height: 60
                    width: height
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
                            text: "Capture image"
                        }
                        onPressed: {
                            captureStatus.source = "qrc:/images/images/Running.gif"
                            loadingAnimation.start()
                            vidstreamproperty.makeShot("/home/curtis/Desktop", "jpg")
                            control.captureImage()
                        }
                    }
                }

                Image {
                    id: captureStatus
                    source: ""
                    height: 50
                    width: height
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
                            control.captureComplete()
                        }
                    }
                }

                Item {
                    id: showPreview
                    height: 60
                    width: height
                    Image {
                        id: previewImage
                        source: "qrc:/images/images/look.png"
                        sourceSize.height: parent.height
                        sourceSize.width: parent.width
                    }
                    MouseArea {
                        id: showPreviewArea
                        anchors.fill: parent
                        hoverEnabled: true

                        ToolTip {
                            parent: showPreviewArea
                            visible: showPreviewArea.containsMouse
                            text:"Show camera preview"
                        }
                        onPressed: { preview.visible = true }
                    }
                }

                Item {
                    id: resetCamera
                    height: 60
                    width: height
                    Image {
                        id: resetImage
                        source: "qrc:/images/images/Refresh_icon.png"
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
                            control.setCameraDiscovery(true)
                            vidstreamproperty.startCam()
                        }
                    }
                }
            }

            Rectangle {
                id: spacer2
                anchors.top: buttonRow.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 10
                height: 20
            }

            Text {
                id: cameraTextStatic
                text: qsTr("Camera Status: ")
                color: "black"
                font.pointSize: 20
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: spacer2.bottom
            }
            Text {
                id: cameraText
                text: qsTr("Loading Camera...")
                color: "black"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: cameraTextStatic.bottom
                font.pointSize: 20

                Connections {
                    target: vidstreamproperty
                    onUnableToOpenCam: {
                        cameraText.text = "Error opening Camera"
                        cameraText.color = "red"
                        control.setCameraDiscovery(false)
                    }
                }
                Connections {
                    target: control
                    onCameraReady: {
                        cameraText.text = "Camera load Success"
                        cameraText.color = "green"
                    }
                }
            }

            Rectangle {
                id: spacer3
                anchors.top: cameraText.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 10
                height: 20
            }

            TextField {
                id: serialField
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: spacer3.bottom
                placeholderText: qsTr("Scan serial number")
            }

            TextField {
                id: macField
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: serialField.bottom
                placeholderText: qsTr("Scan MAC address")
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // Section 2: Last Picture Taken by Camera
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

            StatusText {
                id: statusText
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
                height: parent.height / 1.25
                width: parent.width / 1.25
                fillMode: Image.PreserveAspectFit
            }

            Connections {
                target: vidstreamproperty
                onCaptureSuccess: {
                    scannedImage.source = StandardPaths.standardLocations(StandardPaths.PicturesLocation) + "/latestCapture.jpg"
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // Section 3: Cropped Image
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

            StatusText {
                id: statusText2
                Connections {
                    target: vidstreamproperty
                    onCaptureSuccess: statusText2.visible = false
                    onCaptureFail: { statusText2.color = "red"; statusText2.text = "FAILED TO CAPTURE IMAGE" }
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // Section 4: Text from Cropped image that is going to be used to print label
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

            StatusText {
                id: statusText3
                Connections {
                    target: vidstreamproperty
                    onCaptureSuccess: statusText3.visible = false
                    onCaptureFail: { statusText3.color = "red"; statusText3.text = "FAILED TO CAPTURE IMAGE" }
                }
            }

            Item {
                id: printRegion
                visible: readyToPrint
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                height: parent.height - statusText3.height - 50
                width: parent.width - 50
                Rectangle {
                    id: textToPrintRegion
                    height: parent.height / 2
                    width: 400
                    anchors.left: parent.left
                    anchors.leftMargin: printRegion.width / 8
                    anchors.topMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    color: "white"
                    border.color: "black"
                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 5
                        Row {
                            spacing: 5
                            TextArea {
                                anchors.verticalCenter: parent.verticalCenter
                                text: "SSID 2.4GHz: "
                                font.family: "Helvetica"
                                font.pointSize: 16
                                color: "blue"
                                readOnly: true
                            }
                            TextEdit {
                                id: ssid24
                                anchors.verticalCenter: parent.verticalCenter
                                width: 250
                                text: qsTr(control.ssid24)
                                font.family: "Helvetica"
                                font.pointSize: 14
                                color: "black"
                                onEditingFinished: control.setSsid24Control(text)
                            }
                        }

                        Row {
                            spacing: 5
                            TextArea {
                                anchors.verticalCenter: parent.verticalCenter
                                text: "SSID 5.0GHz: "
                                font.family: "Helvetica"
                                font.pointSize: 16
                                color: "blue"
                                readOnly: true
                            }
                            TextEdit {
                                id: ssid50
                                anchors.verticalCenter: parent.verticalCenter
                                width: 250
                                text: qsTr(control.ssid50)
                                font.family: "Helvetica"
                                font.pointSize: 14
                                color: "black"
                                onEditingFinished: control.setSsid50Control(text)
                            }
                        }

                        Row {
                            spacing: 5
                            TextArea {
                                anchors.verticalCenter: parent.verticalCenter
                                text: "SSID Passphrase: "
                                font.family: "Helvetica"
                                font.pointSize: 16
                                color: "blue"
                                readOnly: true
                            }
                            TextEdit {
                                id: wirelessKey
                                anchors.verticalCenter: parent.verticalCenter
                                width: 250
                                text: qsTr(control.wirelessKey)
                                font.family: "Helvetica"
                                font.pointSize: 14
                                color: "black"
                                onEditingFinished: control.setWirelessKeyControl(text)
                            }
                        }

                        Row {
                            spacing: 5
                            TextArea {
                                anchors.verticalCenter: parent.verticalCenter
                                text: "User Password: "
                                font.family: "Helvetica"
                                font.pointSize: 16
                                color: "blue"
                                readOnly: true
                            }
                            TextEdit {
                                id: homePagePwd
                                anchors.verticalCenter: parent.verticalCenter
                                width: 250
                                text: qsTr(control.usrPwd)
                                font.family: "Helvetica"
                                font.pointSize: 14
                                color: "black"
                                onEditingFinished: control.setUsrPwdControl(text)
                            }
                        }
                    }
                    Connections {
                        target: control
                        onImageProcessingComplete: readyToPrint = true
                    }
                }

                Image {
                    id: printLabelButton
                    anchors.right: parent.right
                    anchors.rightMargin: parent.width / 8
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:/images/images/print.png"
                    ColorOverlay {
                        id: imageColour
                        anchors.fill: printLabelButton
                        source: printLabelButton
                        color: "white"  // make image white to start with
                        SequentialAnimation on color {
                            id: printingAnimation
                            ColorAnimation { to: "orange"; duration: 1500 }
                            ColorAnimation { to: "white"; duration: 800 }
                            loops: Animation.Infinite
                            running: false
                        }
                    }
                    MouseArea {
                        id: printLabelBtnArea
                        anchors.fill: parent
                        hoverEnabled: true

                        ToolTip {
                            parent: printLabelBtnArea
                            visible: printLabelBtnArea.containsMouse
                            text:"Print text to label"
                        }
                        onPressed: {
                            console.log("PRINT LABEL pressed")
                            printingAnimation.start()
                            control.printLabel()
                        }
                    }

                    Connections {
                        target: control
                        onPrintingComplete: {
                            printingAnimation.stop()
                            imageColour.color = "green"
                        }
                        onPrintingError: {
                            printingAnimation.stop()
                            imageColour.color = "red"
                        }
                    }
                }
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
}

