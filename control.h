#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QSharedPointer>

#include "utilities.h"
#include "imageprocessing.h"
#include "labelprinter.h"

#include "cameraproperty.h"
#include "uvccamera.h"
#include "seecam_81.h"
#include "see3cam_130.h"

class Control : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ssid24 READ ssid24Control WRITE setSsid24Control NOTIFY infoChanged)
    Q_PROPERTY(QString ssid50 READ ssid50Control WRITE setSsid50Control NOTIFY infoChanged)
    Q_PROPERTY(QString wirelessKey READ wirelessKeyControl WRITE setWirelessKeyControl NOTIFY infoChanged)
    Q_PROPERTY(QString usrPwd READ usrPwdControl WRITE setUsrPwdControl NOTIFY infoChanged)
    Q_PROPERTY(bool cameraDiscoveryState READ cameraDiscovery WRITE setCameraDiscovery NOTIFY cameraStatusChanged)
    Q_PROPERTY(bool serialNumValid READ validSerial NOTIFY serialValidChanged)
    Q_PROPERTY(bool macAddrValid READ validMac NOTIFY macValidChanged)

public:
    explicit Control(QObject *parent = nullptr);


signals:
    void cameraReady();
    void resetAllContent();
    void serialValid();
    void serialInvalid();
    void macValid();
    void macInvalid();
    void serialValidChanged();
    void macValidChanged();
    void captureImage();
    void captureComplete();
    void imageProcessingComplete();
    void imageProcessingError();
    void printLabel();
    void printingComplete();
    void printingError();
    void infoChanged();
    void cameraStatusChanged();

public slots:
    Q_INVOKABLE bool setFocus();

    Q_INVOKABLE QString serialControl() const;
    Q_INVOKABLE void setSerialControl(const QString &serialControl);

    Q_INVOKABLE QString macControl() const;
    Q_INVOKABLE void setMacControl(const QString &macControl);

    Q_INVOKABLE QString ssid24Control() const;
    Q_INVOKABLE void setSsid24Control(const QString &ssid24Control);

    Q_INVOKABLE QString ssid50Control() const;
    Q_INVOKABLE void setSsid50Control(const QString &ssid50Control);

    Q_INVOKABLE QString wirelessKeyControl() const;
    Q_INVOKABLE void setWirelessKeyControl(const QString &wirelessKeyControl);

    Q_INVOKABLE QString usrPwdControl() const;
    Q_INVOKABLE void setUsrPwdControl(const QString &usrPwdControl);

    Q_INVOKABLE void validateSerial();
    Q_INVOKABLE void validateMac();

    bool cameraDiscovery() const;
    void setCameraDiscovery(bool cameraDiscovery);

    bool validSerial() const;
    void setValidSerial(bool validSerial);

    bool validMac() const;
    void setValidMac(bool validMac);

    void resetContent();

private:
    Utilities m_utilities;
    ImageProcessing *m_imageProcessing;
    LabelPrinter m_labelPrint;
    Cameraproperty m_cameraProperty;
    UvcCamera m_uvc;
    See3CAM_81 m_see3Cam81;
    See3CAM_130 m_see3Cam130;
    QString m_serialControl;
    QString m_macControl;
    QString m_ssid24Control;
    QString m_ssid50Control;
    QString m_wirelessKeyControl;
    QString m_usrPwdControl;
    bool m_cameraDiscovery;
    bool m_validSerial;
    bool m_validMac;
    QString m_selectedCam;
};

#endif // CONTROL_H
