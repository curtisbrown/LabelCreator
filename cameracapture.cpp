#include "cameracapture.h"

#include <QThread>

CameraCapture::CameraCapture(QObject *parent, Utilities *utilities) :
    QObject(parent),
    m_utilities(utilities)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
}

void CameraCapture::captureImage()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    m_utilities->debugLogMessage("Capturing image to print...");
    emit captureDone();
}

void CameraCapture::resetContent()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
}
