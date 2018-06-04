#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <QState>
#include <QFinalState>
#include <QStateMachine>

#include "utilities.h"

class ImageProcessing : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessing(QObject *parent = nullptr, Utilities *utilities = nullptr);

    QString ssid24();
    void setSsid24(const QString &ssid24);

    QString ssid50();
    void setSsid50(const QString &ssid50);

    QString wirelessKey();
    void setWirelessKey(const QString &wirelessKey);

    QString homepagePwd();
    void setHomepagePwd(const QString &homepagePwd);

signals:
    void infoRetrievalComplete();
    void infoRetrievalError();
    void process1ok();
    void process2ok();
    void process3ok();
    void process4ok();
    void process5ok();
    void process6ok();
    void infoProcessed();

public slots:
    void resetContent();
    void processImageStart();
    void processImage();

private:
    Utilities *m_utilities;
    QProcess m_process1, m_process2, m_process3, m_process4, m_process5, m_process6, m_process7;
    QString m_homeDir;
    QString m_ocrDir;
    QString m_ocrSwInstallDir;
    QStringList m_fileContents;
    QString m_ssid24;
    QString m_ssid50;
    QString m_wirelessKey;
    QString m_homepagePwd;

};

#endif // IMAGEPROCESSING_H
