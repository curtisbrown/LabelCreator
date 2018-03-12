/*
 * cameraproperty.cpp -- enumerate the available cameras and filters the e-con camera connected to the device
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

#include "cameraproperty.h"
#include <QDebug>
#include "common_enums.h"

QStringListModel Cameraproperty::modelCam;

Cameraproperty::Cameraproperty()
{
    connect(this,SIGNAL(setFirstCamDevice(int)),&m_vidStr,SLOT(getFirstDevice(int)));
    connect(this,SIGNAL(setCamName(QString)),&m_vidStr,SLOT(getCameraName(QString)));
    /**
     * @brief connect - This signal is used to send the currently selected device to m_uvcCamera.cpp filr
     */
    connect(this,SIGNAL(setCamName(QString)),&m_uvcCam,SLOT(currentlySelectedDevice(QString)));
    /**
     * @brief connect - This signal is used to send the currently selected camera enum to videostreaming.cpp
     */
    connect(&m_uvcCam,SIGNAL(currentlySelectedCameraEnum(CommonEnums::ECameraNames)),&m_vidStr,SLOT(selectedCameraEnum(CommonEnums::ECameraNames)));
    /**
     * @brief connect - This signal is used to send the selected camera enum value to QML for commparision instead of camera name
     */
    connect(&m_uvcCam,SIGNAL(currentlySelectedCameraEnum(CommonEnums::ECameraNames)),this,SLOT(selectedDeviceEnum(CommonEnums::ECameraNames)));
    connect(&m_uvcCam,SIGNAL(hidWarningReceived(QString, QString)),this,SLOT(notifyUser(QString, QString)));
}

/**
 * @brief Cameraproperty::selectedDeviceEnum - This is used to send the Camera enum name to QML
 */
void Cameraproperty::selectedDeviceEnum(CommonEnums::ECameraNames selectedCameraEnum)
{
    emit currentlySelectedCameraEnum((int)selectedCameraEnum);
}

void Cameraproperty::checkforDevice()
{
    qDebug() << Q_FUNC_INFO;

    QDir qDir;

    int deviceBeginNumber,deviceEndNumber;
    m_cameraMap.clear();
    m_deviceNodeMap.clear();
    int deviceIndex = 1;
    m_availableCam.clear();
    if (qDir.cd("/sys/class/video4linux/")) {
        QStringList filters,list;
        filters << "video*";
        qDir.setNameFilters(filters);
        list << qDir.entryList(filters,QDir::Dirs ,QDir::Name);
        std::sort(list.begin(), list.end());
        deviceBeginNumber = list.value(0).mid(5).toInt();   //Fetching all values after "video"
        deviceEndNumber = list.value(list.count()-1).mid(5).toInt();
        for (int qDevCount = deviceBeginNumber; qDevCount <= deviceEndNumber; qDevCount++) {
            QString qTempStr = qDir.path().append("/video" + QString::number(qDevCount));
            if(open("/dev/video" +QString::number(qDevCount),false)) {
                struct v4l2_capability quearyCap;
                if (querycap(quearyCap)) {
                    QString cameraName = (char*)quearyCap.card;
                    if(cameraName.length()>22){
                        cameraName.insert(22,"\n");
                    }
                    m_cameraMap.insert(qDevCount,QString::number(deviceIndex,10));
                    m_deviceNodeMap.insert(deviceIndex,(char*)quearyCap.bus_info);
                    m_availableCam.append(cameraName);
                    deviceIndex++;
                    close();
                } else {
                    qDebug() << "Cannot open device: /dev/video" << qDevCount;
                    return void();
                }
            } else {
                qDebug() << qTempStr << "Device opening failed" << qDevCount;
            }
        }
    } else {
        qDebug() << "/sys/class/video4linux/ path is Not available";
    }

    qDebug() << "Camera devices Connected to System: " << m_availableCam.join(", ");
    m_uvcCam.findEconDevice("video4linux");
    m_availableCam.prepend("----Select Camera Device----");
    modelCam.setStringList(m_availableCam);
    m_uvcCam.findEconDevice("hidraw");
}

void Cameraproperty::setCurrentDevice(QString deviceIndex,QString deviceName)
{
    qDebug() << Q_FUNC_INFO;

    if(deviceIndex.isEmpty() || deviceName.isEmpty()) {
        emit setFirstCamDevice(-1);
        return;
    }

    qDebug() << "Selected Device is: " << deviceName;
    if(deviceName == "----Select camera----") {
        emit setFirstCamDevice(-1);
    } else {
        emit setFirstCamDevice(m_cameraMap.key(deviceIndex));
        emit setCamName(deviceName);
        m_uvcCam.getDeviceNodeName(m_deviceNodeMap.value(deviceIndex.toInt()));
    }
}

void Cameraproperty::openHIDDevice(QString deviceName)
{
    m_uvcCam.exitExtensionUnit();
    deviceName.remove(QRegExp("[\n\t\r]"));
    bool hidInit = m_uvcCam.initExtensionUnit(deviceName);
    if(hidInit)
        emit initExtensionUnitSuccess();
    else
        qDebug() << "ERROR opening HID device";
}

void Cameraproperty::closeLibUsbDeviceAscella(){
    m_uvcCam.exitExtensionUnitAscella();
}

void Cameraproperty::notifyUser(QString title, QString text){
    emit notifyUserInfo(title, text);
}
