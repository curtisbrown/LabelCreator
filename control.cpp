#include "control.h"

#include <QtMultimedia>

Control::Control(QObject *parent) :
    QObject(parent),
    m_utilities(this),
    m_cameraCapture(this, &m_utilities),
    m_labelPrint(this, &m_utilities)
{
    // Connections
    connect(this, &Control::captureImage, &m_cameraCapture, &CameraCapture::captureImage);
    connect(this, &Control::resetAllContent, &m_cameraCapture, &CameraCapture::resetContent);
    connect(&m_cameraCapture, &CameraCapture::captureDone, this, &Control::captureComplete);

    connect(this, &Control::printLabel, &m_labelPrint, &LabelPrinter::printLabel);
    connect(this, &Control::resetAllContent, &m_labelPrint, &LabelPrinter::resetContent);
    connect(&m_labelPrint, &LabelPrinter::printDone, this, &Control::printingComplete);
    connect(&m_labelPrint, &LabelPrinter::printFailed, this, &Control::printingError);

    m_cameraProperty.checkforDevice();
    m_cameraProperty.setCurrentDevice("1", "See3CAM_81");
    m_cameraProperty.openHIDDevice("See3CAM_81 ");
    m_uvc.getFirmWareVersion();
}

bool Control::setFocus()
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);

    m_see3Cam81.setFocusMode(See3CAM_81::MANUAL_FOCUS_81);

    if (m_see3Cam81.setFocusPosition(311)
            && m_see3Cam81.setEffectMode(See3CAM_81::EFFECT_GRAYSCALE)) {
        m_utilities.debugLogMessage("SUCCESS setting zoom and greyscale");
        return true;
    } else {
        m_utilities.debugLogMessage("ERROR setting camera values");
        return false;
    }
}
