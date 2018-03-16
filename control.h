#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QSharedPointer>

#include "utilities.h"
#include "cameracapture.h"
#include "labelprinter.h"

#include "cameraproperty.h"
#include "uvccamera.h"
#include "seecam_81.h"

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);

signals:
    void resetAllContent();
    void captureImage();
    void captureComplete();
    void printLabel();
    void printingComplete();
    void printingError();

public slots:
    Q_INVOKABLE bool setFocus();

private:
    Utilities m_utilities;
    CameraCapture m_cameraCapture;
    LabelPrinter m_labelPrint;
    Cameraproperty m_cameraProperty;
    UvcCamera m_uvc;
    See3CAM_81 m_see3Cam81;

};

#endif // CONTROL_H
