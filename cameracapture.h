#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include <QObject>

#include "utilities.h"

class CameraCapture : public QObject
{
    Q_OBJECT
public:
    explicit CameraCapture(QObject *parent = nullptr, Utilities *utilities = nullptr);

signals:
    void captureDone();

public slots:
    void captureImage();
    void resetContent();

private:
    Utilities *m_utilities;
};

#endif // CAMERACAPTURE_H
