/*
 * cameraproperty.h -- enumerate the available cameras and filters the e-con camera connected to the device
 * Copyright © 2015  e-con Systems India Pvt. Limited
 *
 * This file is part of Qtcam.
 *
 * Qtcam is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Qtcam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qtcam. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CAMERAPROPERTY_H
#define CAMERAPROPERTY_H
#include <QQuickPaintedItem>
#include <QQuickImageProvider>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QDir>
#include <QStringList>
#include <QStringListModel>
#include <QQmlContext>
#include <QStandardItemModel>
#include <QTimer>

#include "v4l2-api.h"
#include "uvccamera.h"
#include "videostreaming.h"
#include "libudev.h"

class Cameraproperty : public QObject, public v4l2
{
    Q_OBJECT

public:
    Cameraproperty();

    static QStringListModel modelCam;

signals:
    void setFirstCamDevice(int);
    void setCamName(QString);
    /**
     * @brief currentlySelectedCameraEnum - This signal is used to emit selected camera enum value to qtcam.qml file
     */
    void currentlySelectedCameraEnum(int selectedDevice);
    /**
     * @brief initExtensionUnitSuccess - This signal is used to emit after HID initialization is success
     */
    void initExtensionUnitSuccess();

    // TO BE DELETED
    void notifyUserInfo(QString title, QString text);

public slots:
    /**
     * @brief Check e-con Cameras
     *  - List all the camera devices detected in the system
     *  - All the camera device name are listed in a QStringlist variable
     *  - E-con camera will be filtered from the available list
     *  - Flitered list is displayed to the view
     */
    void checkforDevice();

    /**
     * @brief setCurrentDevice
     *  - Used to get device index and pass it to the signals
     * @param
     *  deviceIndex - Selected from the UI, passed to this function
     *  deviceName - Selected from the UI, passed to this function
     */
    void setCurrentDevice(QString,QString);

    /**
     * @brief selectedDeviceEnum - This slot contains the selected camera enum
     */
    void selectedDeviceEnum(CommonEnums::ECameraNames selectedCameraEnum);
    /**
     * @brief openHIDDevice - This slot is used to open HID device
     */
    bool openHIDDevice(QString deviceName);

    // TO BE DELETED
    void notifyUser(QString title, QString text);


private:
    UvcCamera m_uvcCam;
    Videostreaming m_vidStr;

    QStringList m_availableCam;
    QMap<int, QString> m_cameraMap;
    QMap<int, QString> m_deviceNodeMap;
};


#endif // CAMERAPROPERTY_H
