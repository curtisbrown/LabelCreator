import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtMultimedia 5.9
import com.ctdi.labelcreator.control 1.0

import econ.camera.property 1.0
import econ.camera.stream 1.0

Window {
    id: mainWindow
    visible: true
    width: 1024
    height: 768
    title: qsTr("CTDI Label Creator")
    property int buttonMargin: 20
    property bool animateScan : false
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
            width: 140
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                id: text
                text: qsTr("Scan Label")
                color: "blue"
                anchors.centerIn: parent
            }

            onPressed: {
                scanStatus.source = "qrc:/images/images/Running.gif"
                animateScan = true
                control.captureImage()
            }
        }

        Image {
            id: scanStatus
            source: ""
            anchors.right: scanButton.right
            anchors.verticalCenter: parent.verticalCenter
            RotationAnimator {
                target: scanStatus;
                from: 0;
                to: 360;
                duration: 2000;
                loops: Animation.Infinite
                running: animateScan
            }
        }

        Connections {
            target: control
            onCaptureComplete: { scanStatus.source = "qrc:/images/images/pass.png"; animateScan = false }
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
            Videostreaming {
                id: vidstreamproperty
                focus: true
                //onTitleTextChanged:{
                    enabled: true
                    //captureBtnEnable(true)
                    //videoRecordBtnEnable(true)
                    //webcamKeyAccept: true
                    //keyEventFiltering: false
                    //messageDialog.title = _title.toString()
                    //messageDialog.text = _text.toString()
                    //messageDialog.visible = true
                    //if(mouseClickCap){
                        //Added by Sankari : 08 Mar 2017
                        //Enable camera settings/extension settings tab after capturing image
                      //  enableAllSettingsTab()
                      //  mouseClickCap = false
                    //}
                //}
                onEnableRfRectBackInPreview:{
                    afterBurst() // signal to do anything need to do after capture continuous[burst] shots.
                }

                // Enable Face detection rect in preview
                onEnableFactRectInPreview:{
                    enableFaceRectafterBurst()
                }

                onNewControlAdded: {
                    setControlValues(ctrlName.toString(),ctrlType,ctrlMinValue,ctrlMaxValue, ctrlStepSize, ctrlDefaultValue,ctrlID);
                }

                onDeviceUnplugged: {
                    captureBtnEnable(false)
                    videoRecordBtnEnable(false)
                    keyEventFiltering = true
                    //statusText = ""
                    messageDialog.title = _title.toString()
                    messageDialog.text = _text.toString()
                    messageDialog.open()
                    cameraDeviceUnplugged();
                    device_box.oldIndex = 0
                    device_box.currentIndex = 0
                    disableImageSettings();
                    // Added by Sankari: 25 May 2017. When device is unplugged, make preview area disabled
                    vidstreamproperty.enabled = false

                    if(captureVideoRecordRootObject.recordStopBtnVisible) {
                        //statusText = "Saving..."
                        vidstreamproperty.recordStop()
                        captureVideoRecordRootObject.videoTimerUpdate(false)
                        messageDialog.title = qsTr("Saved")
                        messageDialog.text = qsTr("Video saved in the location:"+videofileName)
                        messageDialog.open()
                        videoPropertyItemEnable(true)
                        stillPropertyItemEnable(true)
                        device_box.enabled = true
                        device_box.opacity = 1
                        videoRecordBtnVisible(true)
                        uvc_settings.enabled = true
                        uvc_settings.opacity = 1
                    }
                    if(sideBarItems.visible){ // only when side bar items visible
                        //When device is unplugged,need to destroy the active camera qml and create default qml file
                        if(see3cam){
                            see3cam.destroy()
                            see3cam = Qt.createComponent("../UVCSettings/others/others.qml").createObject(root)
                            see3cam.visible = !cameraColumnLayout.visible
                            extensionTabVisible(see3cam.visible)
                        }
                    }
                }

                onLogDebugHandle: {
                    camproperty.logDebugWriter(_text.toString())
                }

                onLogCriticalHandle: {
                    camproperty.logCriticalWriter(_text.toString())
                }

                onAverageFPS: {
                    if(device_box.opacity === 0.5)
                    {
                        //statusText = "Recording..." + " " + "Current FPS: " + fps + " Preview Resolution: "+ vidstreamproperty.width +"x"+vidstreamproperty.height + " " + "Color Format: " + videoSettingsRootObject.videoColorComboText
                    }
                    else
                    {
                        //statusText = "Current FPS: " + fps + " Preview Resolution: "+ vidstreamproperty.width +"x"+vidstreamproperty.height + " " + stillSettingsRootObject.captureTime + " " + "Color Format: " + videoSettingsRootObject.videoColorComboText
                    }
                }

                onDefaultFrameSize: {
                    if(m_Snap) {
                        setColorComboOutputIndex(false,outputIndexValue)
                    }
                    setVideoColorComboOutputIndex(false,outputIndexValue)
                    vidstreamproperty.width = defaultWidth
                    vidstreamproperty.height = defaultHeight
                }

                onDefaultStillFrameSize: {
                    setColorComboOutputIndex(false,outputIndexValue)
                }

                onDefaultOutputFormat: {
                    if(m_Snap){
                        setColorComboOutputIndex(true,formatIndexValue)
                    }
                    if(!stillPreview){
                        setVideoColorComboOutputIndex(true,formatIndexValue)
                    }
                }

                onDefaultFrameInterval:{
                    videoFrameInterval(frameInterval)
                }
                onRcdStop: {
                    recordFailedDialog.title = "Failed"
                    recordFailedDialog.text = recordFail
                    recordFailedDialog.open()
                    vidstreamproperty.recordStop()
                    captureVideoRecordRootObject.videoTimerUpdate(false)
                }
                onCaptureSaveTime: {
                    stillSettingsRootObject.startCaptureTimer(saveTime);
                }

                onVideoRecord: {
                    videofileName = fileName
                }

                onStillSkipCount:{
                    frameSkipCount(stillResoln, videoResoln);
                }

                onStillSkipCountWhenFPSChange:{
                    frameSkipCountWhenFPSChange(fpsChange)
                }

                // Added by Sankari - get FPS list
                onSendFPSlist:{
                    availableFpslist = fpsList;
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onReleased:
                    {
                        if (mouse.button === Qt.LeftButton){
                            if(closeSideBarClicked){
                                captureRecordWhenSideBarItemsClosed()
                            }
                            else{
                                if(captureVideoRecordRootObject.captureBtnVisible){
                                    mouseClickCapture()
                                } else if(captureVideoRecordRootObject.recordBtnVisible){
                                    videoRecordBegin()
                                } else if(captureVideoRecordRootObject.recordStopBtnVisible){
                                    videoSaveVideo()
                                }
                            }
                        }else if(mouse.button === Qt.RightButton){
                            // passing mouse x,y cororinates, preview width and height
                            mouseRightClicked(mouse.x, mouse.y, vidstreamproperty.width, vidstreamproperty.height)
                        }
                    }
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
                    target: control
                    onCaptureComplete: statusText.visible = false
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
                target: control
                onCaptureComplete: scannedImage.source = "file:/home/curtis/Pictures/Tux.png"
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
        // Text from Cropped Imgae that is going to be used to print label
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

//    Rectangle {
//        id: pictureContainer
//        color: "grey"
//        Text {
//            id: statusText
//            text: qsTr("Scan label to start")
//            color: "white"
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.verticalCenter: parent.verticalCenter
//            font.pointSize: 20
//            SequentialAnimation on color {
//                ColorAnimation { to: "orange"; duration: 1500 }
//                ColorAnimation { to: "white"; duration: 800 }
//                loops: Animation.Infinite
//            }

//            Connections {
//                target: control
//                onCaptureComplete: statusText.visible = false
//            }
//        }
//        Image {
//            id: scannedImage
//            source: ""
//            anchors.centerIn: parent
//            height: parent.height / 2
//            width: parent.width / 2
//        }

//        Connections {
//            target: control
//            onCaptureComplete: scannedImage.source = "file:/C:/Users/Curtis/Pictures/porsche.jpg"
//        }
//    }

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
