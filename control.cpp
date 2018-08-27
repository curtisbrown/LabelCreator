#include "control.h"

#include <QtMultimedia>

Control::Control(QObject *parent) :
    QObject(parent),
    m_utilities(this),
    m_imageProcessing(new ImageProcessing(this, &m_utilities)),
    m_labelPrint(this, &m_utilities),
    m_serialControl(""),
    m_ssid24Control(""),
    m_ssid50Control(""),
    m_wirelessKeyControl(""),
    m_usrPwdControl(""),
    m_cameraDiscovery(false),
    m_selectedCam("")
{
    // Connections
    connect(this, &Control::resetAllContent, this, &Control::resetContent);

    connect(this, &Control::resetAllContent, m_imageProcessing, &ImageProcessing::resetContent);
    connect(this, &Control::captureComplete, m_imageProcessing, &ImageProcessing::processImage);
    connect(m_imageProcessing, &ImageProcessing::infoRetrievalComplete, this, [=]() {
        setSsid24Control(m_imageProcessing->ssid24());
        setSsid50Control(m_imageProcessing->ssid50());
        setWirelessKeyControl(m_imageProcessing->wirelessKey());
        setUsrPwdControl(m_imageProcessing->homepagePwd());
        emit this->imageProcessingComplete();
    });
    connect(m_imageProcessing, &ImageProcessing::infoRetrievalError, this, &Control::imageProcessingError);

    connect(this, &Control::resetAllContent, &m_labelPrint, &LabelPrinter::resetContent);
    connect(this, &Control::printLabel, [=]() {
        this->m_labelPrint.startFtpUpload(this->m_serialControl,
                                          this->m_ssid24Control,
                                          this->m_wirelessKeyControl,
                                          this->m_usrPwdControl);
    });
    connect(&m_labelPrint, &LabelPrinter::printDone, this, &Control::printingComplete);
    connect(&m_labelPrint, &LabelPrinter::printFailed, this, &Control::printingError);

    m_cameraProperty.checkforDevice();
    m_cameraProperty.setCurrentDevice("1", "See3CAM_130");
    if (m_cameraProperty.openHIDDevice("See3CAM_130")) {
        m_uvc.getFirmWareVersion();
        m_cameraDiscovery = true;
        m_selectedCam = "See3CAM_130";
    } else {
        m_utilities.debugLogMessage("ERROR failed to open HID Device, communication with camera unavailable");
    }
}

bool Control::setFocus()
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);

    if (!m_cameraDiscovery) {
       m_utilities.debugLogMessage("ERROR: Not attempting to set focus");
       return false;
    } else {

        if (m_selectedCam == "See3CAM_130") {
            m_see3Cam130.setAutoFocusMode(See3CAM_130::Continuous);
            if (m_see3Cam130.setEffectMode(See3CAM_130::EFFECT_GREYSCALE)) {
                m_utilities.debugLogMessage("SUCCESS setting zoom and greyscale");
                emit cameraReady();
                return true;
            } else {
                m_utilities.debugLogMessage("ERROR setting GREYSCALE");
                return false;
            }
        } else {    // Currently only supporting 2 camera types
            m_see3Cam81.setFocusMode(See3CAM_81::MANUAL_FOCUS_81);
            if (m_see3Cam81.setFocusPosition(270)) {
                QThread::msleep(3000);
                if (m_see3Cam81.setEffectMode(See3CAM_81::EFFECT_GRAYSCALE)) {
                    m_utilities.debugLogMessage("SUCCESS setting zoom and greyscale");
                    emit cameraReady();
                    return true;
                } else {
                    m_utilities.debugLogMessage("ERROR setting GREYSCALE");
                    return false;
                }
            } else {
                m_utilities.debugLogMessage("ERROR setting ZOOM");
                return false;
            }
        }
    }
}

QString Control::usrPwdControl() const
{
    return m_usrPwdControl;
}

void Control::setUsrPwdControl(const QString &usrPwdControl)
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);
    m_usrPwdControl = usrPwdControl;
    m_imageProcessing->setHomepagePwd(m_usrPwdControl);
    emit infoChanged();
}

bool Control::cameraDiscovery() const
{
    return m_cameraDiscovery;
}

void Control::setCameraDiscovery(bool cameraDiscovery)
{
    m_cameraDiscovery = cameraDiscovery;
}

void Control::resetContent()
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);

    m_serialControl.clear();
    m_ssid24Control.clear();
    m_ssid50Control.clear();
    m_usrPwdControl.clear();
    m_wirelessKeyControl.clear();
}

QString Control::serialControl() const
{
    return m_serialControl;
}

void Control::setSerialControl(const QString &serialControl)
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);
    m_serialControl = serialControl;
    m_utilities.debugLogMessage(m_serialControl);
}

QString Control::wirelessKeyControl() const
{
    return m_wirelessKeyControl;
}

void Control::setWirelessKeyControl(const QString &wirelessKeyControl)
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);
    m_wirelessKeyControl = wirelessKeyControl;
    m_imageProcessing->setWirelessKey(m_wirelessKeyControl);
    emit infoChanged();
}

QString Control::ssid50Control() const
{
    return m_ssid50Control;
}

void Control::setSsid50Control(const QString &ssid50Control)
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);
    m_ssid50Control = ssid50Control;
    m_imageProcessing->setSsid50(m_ssid50Control);
    emit infoChanged();
}

QString Control::ssid24Control() const
{
    return m_ssid24Control;
}

void Control::setSsid24Control(const QString &ssid24Control)
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);
    m_ssid24Control = ssid24Control;
    m_imageProcessing->setSsid24(m_ssid24Control);
    emit infoChanged();
}
