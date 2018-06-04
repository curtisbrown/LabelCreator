#include "imageprocessing.h"
#include <QDir>
#include <QFile>
#include<QTextStream>

ImageProcessing::ImageProcessing(QObject *parent, Utilities *utilities) :
    QObject(parent),
    m_utilities(utilities),
    m_homeDir(QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory)),
    m_ocrDir(m_homeDir + "OCR"),
    m_ocrSwInstallDir(m_ocrDir + "/ocropy-master"),
    m_ssid24("x"),
    m_ssid50("y"),
    m_wirelessKey("z"),
    m_homepagePwd("1")
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    m_utilities->debugLogMessage(m_homeDir);
    m_utilities->debugLogMessage(m_ocrSwInstallDir);
}

void ImageProcessing::resetContent()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    m_fileContents.clear();
    m_ssid24.clear();
    m_ssid50.clear();
    m_wirelessKey.clear();
    m_homepagePwd.clear();
}

void ImageProcessing::processImageStart()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);

    // Convert image to BMP format
    m_process1.start(QString("mogrify -format bmp %1/latestCapture.jpeg -path %1").arg(m_ocrDir));
    m_process1.waitForFinished(5000);

    m_process2.start(QString("python %1/universal.py %2").arg(m_ocrDir).arg(m_homeDir));
    m_process2.waitForFinished(10000);

    // Remove Book contents of old capture
    m_process3.start(QString("rm -r %1/book").arg(m_ocrSwInstallDir));
    m_process3.waitForFinished(5000);

    m_process4.start(QString("python %1/ocropus-nlbin -n %1/tests/crop.tif -o %1/book").arg(m_ocrSwInstallDir));
    m_process4.waitForFinished(5000);

    m_process5.start(QString("python %1/ocropus-gpageseg -n --minscale 11.0 --maxcolseps 0 %1/book/0001.bin.png").arg(m_ocrSwInstallDir));
    m_process5.waitForFinished(5000);

    m_process6.start(QString("python %1/ocropus-rpred -m arial2.pyrnn.gz %1/book/0001/*.png").arg(m_ocrSwInstallDir));
    m_process6.waitForFinished(5000);

    //m_process.start(QString("python %1/ocropus-rpred -m sansculottes.pyrnn.gz %1/book/0001/*.png").arg(m_ocrSwInstallDir));
    //m_process.waitForFinished(5000);

    //m_process.start(QString("python %1/ocropus-rpred -m en-default.pyrnn.gz %1/book/0001/*.png").arg(m_ocrSwInstallDir));
    //m_process.waitForFinished(5000);

    //m_process.start(QString("python %1/ocropus-rpred -n -m %1/consolas1.pyrnn.gz %1/book/0001/*.png").arg(m_ocrSwInstallDir));
    //m_process.waitForFinished(5000);

    QDir files(m_ocrSwInstallDir + "/book/0001");
    QStringList tempList;
    foreach (QString name, files.entryList()) {
        QFile file(m_ocrSwInstallDir + "/book/0001/" + name);
        if (file.fileName().endsWith(".txt")) {
            if (file.open(QIODevice::ReadOnly)) {

                QTextStream in(&file);
                QString line = in.readAll();
                tempList.append(line);

                file.close();
            } else {
                m_utilities->debugLogMessage("ERROR: Couldn't open file");
            }

        }
    }

    if (tempList.isEmpty()) {
        m_utilities->debugLogMessage("ERROR: No information retrieved via OCR SW");
        emit infoRetrievalError();
    } else {
        foreach (QString line, tempList) {
            m_utilities->debugLogMessage(line);
        }

        if (tempList.count() == 5) {
            // remove first as this is rubbish information
            tempList.removeFirst();
            foreach (QString entry, tempList) {
                m_fileContents.append(entry.remove("\n"));
            }

            setSsid24(m_fileContents.at(0));
            setSsid50(m_fileContents.at(1));
            setWirelessKey(m_fileContents.at(2));
            setHomepagePwd(m_fileContents.at(3));
            emit infoRetrievalComplete();
        } else {
            emit infoRetrievalError();
        }
    }
}

QString ImageProcessing::ssid24() { return m_ssid24; }
void ImageProcessing::setSsid24(const QString &ssid24)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    if (m_ssid24 != ssid24) {
        m_utilities->debugLogMessage(QString("Setting SSID 2.4 to %1").arg(ssid24));
        m_ssid24 = ssid24;
    }
}

QString ImageProcessing::ssid50() { return m_ssid50; }
void ImageProcessing::setSsid50(const QString &ssid50)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    if (m_ssid50 != ssid50) {
        m_utilities->debugLogMessage(QString("Setting SSID 5.0 to %1").arg(ssid50));
        m_ssid50 = ssid50;
    }
}

QString ImageProcessing::wirelessKey() { return m_wirelessKey; }
void ImageProcessing::setWirelessKey(const QString &wirelessKey)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    if (m_wirelessKey != wirelessKey) {
        m_utilities->debugLogMessage(QString("Setting SSID Wireless Key to %1").arg(wirelessKey));
        m_wirelessKey = wirelessKey;
    }
}

QString ImageProcessing::homepagePwd() { return m_homepagePwd; }
void ImageProcessing::setHomepagePwd(const QString &homepagePwd)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    if (m_homepagePwd != homepagePwd) {
        m_utilities->debugLogMessage(QString("Setting homepagePwd to %1").arg(homepagePwd));
        m_homepagePwd = homepagePwd;
    }
}

void ImageProcessing::processImage()
{
    QStateMachine *stateMachine = new QStateMachine(this);
    QState *convertImgtoBmp = new QState(stateMachine);
    QState *universal = new QState(stateMachine);
    QState *removeOldBook = new QState(stateMachine);
    QState *ocropus_nlbin = new QState(stateMachine);
    QState *ocropus_gpageseg = new QState(stateMachine);
    QState *ocropus_rpred = new QState(stateMachine);
    QState *organiseInformationToPrint = new QState(stateMachine);
    QFinalState *done = new QFinalState(stateMachine);
    stateMachine->setInitialState(convertImgtoBmp);

    connect(convertImgtoBmp, &QState::entered, this, [=]() {
        m_utilities->debugLogMessage("......................convertImgtoBmp");
        m_process1.start(QString("mogrify -format bmp %1/latestCapture.jpeg -path %1").arg(m_ocrDir));
        m_process1.waitForStarted(5000);
    });
    connect(&m_process1, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit and exitCode == 0)
            emit process1ok();
    });
    connect(universal, &QState::entered, this, [=]() {
        m_utilities->debugLogMessage("......................universal");
        m_process2.start(QString("python %1/universal.py %2").arg(m_ocrDir).arg(m_homeDir));
        m_process2.waitForStarted(10000);
    });
    connect(&m_process2, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit and exitCode == 0)
            emit process2ok();
    });
    connect(removeOldBook, &QState::entered, this, [=]() {
        m_utilities->debugLogMessage("......................removeOldBook");
        m_process3.start(QString("rm -r %1/book").arg(m_ocrSwInstallDir));
        m_process3.waitForStarted(5000);
    });
    connect(&m_process3, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit and exitCode == 0)
            emit process3ok();
    });
    connect(ocropus_nlbin, &QState::entered, this, [=]() {
        m_utilities->debugLogMessage("......................ocropus_nlbin");
        m_process4.start(QString("python %1/ocropus-nlbin -n %1/tests/crop.tif -o %1/book").arg(m_ocrSwInstallDir));
        m_process4.waitForStarted(5000);
    });
    connect(&m_process4, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit and exitCode == 0)
            emit process4ok();
    });
    connect(ocropus_gpageseg, &QState::entered, this, [=]() {
        m_utilities->debugLogMessage("......................ocropus_gpageseg");
        m_process5.start(QString("python %1/ocropus-gpageseg -n --minscale 11.0 --maxcolseps 0 %1/book/0001.bin.png").arg(m_ocrSwInstallDir));
        m_process5.waitForStarted(5000);
    });
    connect(&m_process5, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit and exitCode == 0)
            emit process5ok();
    });
    connect(ocropus_rpred, &QState::entered, this, [=]() {
        m_utilities->debugLogMessage("......................ocropus_rpred");
        m_process7.start(QString("python %1/ocropus-rpred -m arial2.pyrnn.gz %1/book/0001/*.png").arg(m_ocrSwInstallDir));
        m_process7.waitForFinished(5000);
        //m_process6.start(QString("python %1/ocropus-rpred -m sansculottes.pyrnn.gz %1/book/0001/*.png").arg(m_ocrSwInstallDir));
        m_process6.start(QString("python %1/ocropus-rpred -n -m %1/consolas1.pyrnn.gz %1/book/0001/*.png").arg(m_ocrSwInstallDir));
        //m_process6.start(QString("python %1/ocropus-rpred -m en-default.pyrnn.gz %1/book/0001/*.png").arg(m_ocrSwInstallDir));
        m_process6.waitForStarted(5000);
    });
    connect(&m_process6, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),[=](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit and exitCode == 0)
            emit process6ok();
    });
    connect(organiseInformationToPrint, &QState::entered, this, [=]() {
        QDir files(m_ocrSwInstallDir + "/book/0001");
        QStringList tempList;
        foreach (QString name, files.entryList()) {
            QFile file(m_ocrSwInstallDir + "/book/0001/" + name);
            if (file.fileName().endsWith(".txt")) {
                if (file.open(QIODevice::ReadOnly)) {

                    QTextStream in(&file);
                    QString line = in.readAll();
                    tempList.append(line);

                    file.close();
                } else {
                    m_utilities->debugLogMessage("ERROR: Couldn't open file");
                }

            }
        }

        if (tempList.isEmpty()) {
            m_utilities->debugLogMessage("ERROR: No information retrieved via OCR SW");
        } else {
            foreach (QString line, tempList) {
                m_utilities->debugLogMessage(line);
            }

            if (tempList.count() == 5) {
                // remove first as this is rubbish information
                tempList.removeFirst();
                foreach (QString entry, tempList) {
                    m_fileContents.append(entry.remove("\n"));
                }

                setSsid24(m_fileContents.at(0));
                setSsid50(m_fileContents.at(1));
                setWirelessKey(m_fileContents.at(2));
                setHomepagePwd(m_fileContents.at(3));
            }
        }
        emit infoProcessed();
    });
    connect(this, &ImageProcessing::infoProcessed, this, [=]() {
        if (m_ssid24.isEmpty() || m_ssid50.isEmpty() || m_wirelessKey.isEmpty() || m_homepagePwd.isEmpty()) {
            emit infoRetrievalError();
        } else {
            emit infoRetrievalComplete();
        }
    });
    connect(done, &QFinalState::entered, this, [=]() {
        m_utilities->debugLogMessage("......................Image processing completed");
    });

    convertImgtoBmp->addTransition(this, &ImageProcessing::process1ok, universal);
    universal->addTransition(this, &ImageProcessing::process2ok, removeOldBook);
    removeOldBook->addTransition(this, &ImageProcessing::process3ok, ocropus_nlbin);
    ocropus_nlbin->addTransition(this, &ImageProcessing::process4ok, ocropus_gpageseg);
    ocropus_gpageseg->addTransition(this, &ImageProcessing::process5ok, ocropus_rpred);
    ocropus_rpred->addTransition(this, &ImageProcessing::process6ok, organiseInformationToPrint);
    organiseInformationToPrint->addTransition(this, &ImageProcessing::infoRetrievalComplete, done);
    organiseInformationToPrint->addTransition(this, &ImageProcessing::infoRetrievalError, done);

    stateMachine->start();
}
