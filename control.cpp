#include "control.h"

#include <QtMultimedia>

Control::Control(QObject *parent) :
    QObject(parent),
    m_utilities(this),
    m_cameraCapture(parent, &m_utilities),
    m_labelPrint(parent, &m_utilities)
{

    // Connections
    connect(this, &Control::captureImage, &m_cameraCapture, &CameraCapture::captureImage);
    connect(this, &Control::resetAllContent, &m_cameraCapture, &CameraCapture::resetContent);
    connect(&m_cameraCapture, &CameraCapture::captureDone, this, &Control::captureComplete);

    connect(this, &Control::printLabel, &m_labelPrint, &LabelPrinter::printLabel);
    connect(this, &Control::resetAllContent, &m_labelPrint, &LabelPrinter::resetContent);
    connect(&m_labelPrint, &LabelPrinter::printDone, this, &Control::printingComplete);

    m_cameraProperty.checkforDevice();
}
