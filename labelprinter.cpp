#include "labelprinter.h"

LabelPrinter::LabelPrinter(QObject *parent, Utilities *utilities)
    : QObject(parent)
    , m_utilities(utilities)
    , m_socket(new QTcpSocket(this))
    , m_hostAddress("172.168.1.200")
    , m_hostPort(9999)
    , m_connected(false)
    , m_host("10.40.10.165")
    , m_user("ctdi")
    , m_password("ctdi")
    , m_dataBuffer("")

{
    m_utilities->debugLogMessage(Q_FUNC_INFO);

    m_timer.setInterval(3000);
    m_timer.setSingleShot(true);

    // Connections
    connect(this, &LabelPrinter::uploadComplete, &m_timer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(&m_timer, &QTimer::timeout, this, &LabelPrinter::checkPrintStatus);
    connect(this, &LabelPrinter::uploadFail, this, &LabelPrinter::printFailed);

    connect(m_socket, &QTcpSocket::connected, this, [=]() {
        m_utilities->debugLogMessage("Successfully connected to the host");
        m_connected = true;
    });
    connect(m_socket, &QTcpSocket::readyRead, this, [=]() {
        QString data = m_socket->readAll();
        m_utilities->debugLogMessage(QString("Data recevied from socket: %1").arg(data));
        if (data.contains("ACK\n"))
            m_utilities->debugLogMessage("Received ACK Packet");
        else
            m_utilities->debugLogMessage("Unknown data received, doing nothing");
    });
    connect(m_socket, &QTcpSocket::disconnected, this, [=]() {
        m_utilities->debugLogMessage("Host has disconnected");
        m_connected = false;
    });
    connect(m_socket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, [=]() {
        m_utilities->debugLogMessage("ERROR: Socket error, lost connection to Server");
        m_socket->disconnectFromHost();
        m_connected = false;
        m_socket->abort();
    });

    connect(&m_curlProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &LabelPrinter::ftpUploadStatus);
    connect(&m_printProcess, &QProcess::readyRead, [=]() {
        // Copy printer data
        this->m_dataBuffer += this->m_printProcess.readAll();
    });
    connect(&m_printProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &LabelPrinter::checkPrintStatus);
}

LabelPrinter::~LabelPrinter()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    m_socket->close();
}

void LabelPrinter::resetContent()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    if (!m_connected) {
        connectToPrinterServer();
    } else {
        m_serial.clear();
        m_dataBuffer.clear();
    }
}

void LabelPrinter::connectToPrinterServer()
{
#ifdef QT_DEBUG
    m_utilities->debugLogMessage("Not Attempting to connect to server while in DEBUG mode");
#else
    m_utilities->debugLogMessage("Attempting to connect to Server");
    m_socket->abort();
    m_socket->connectToHost(m_hostAddress, m_hostPort);
#endif
}

bool LabelPrinter::createFileToUpload(QString serial, QString ssid, QString wifiKey, QString password)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);

    QFile bartenderFile(serial + ".pr1");
    QString data = password + "," + ssid.right(4) + "," + wifiKey;

    if (bartenderFile.exists())
        bartenderFile.remove();

    m_utilities->debugLogMessage("Writing content to file");
    if (bartenderFile.open(QIODevice::WriteOnly)) {
        QTextStream in(&bartenderFile);
        in << data;

        bartenderFile.close();
        return true;
    } else {
        m_utilities->debugLogMessage("ERROR opening file");
        return false;
    }
}

void LabelPrinter::startFtpUpload(QString serial, QString ssid, QString wifiKey, QString password)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);

    // Set serial for checking print status later
    m_serial = serial;

    if (createFileToUpload(serial, ssid, wifiKey, password)) {
        m_curlProcess.start(QString("curl --connect-timeout 25 -T %1.txt ftp://%2 --user \"%3:%4\"").arg(serial).arg(m_host).arg(m_user).arg(m_password));
        if (m_curlProcess.waitForStarted(2000))
            m_utilities->debugLogMessage("FTP upload started");
        else
            m_utilities->debugLogMessage("FTP upload failed to start");
    }
}

void LabelPrinter::ftpUploadStatus(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        m_utilities->debugLogMessage("FTP upload completed");
        emit uploadComplete();
    } else {
        m_utilities->debugLogMessage("FTP upload ERROR");
        emit uploadFail();
    }
}

void LabelPrinter::startCheckPrint()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    m_curlProcess.start(QString("curl -u %1:%2 'ftp://%3:/%4.done' -v").arg(m_user).arg(m_password).arg(m_host).arg(m_serial));
    if (m_curlProcess.waitForStarted(2000))
        m_utilities->debugLogMessage("FTP upload started");
    else
        m_utilities->debugLogMessage("FTP upload failed to start");
}

void LabelPrinter::checkPrintStatus()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);

    if (m_dataBuffer.contains(QString("Successfully transferred /%1.done").arg(m_serial))) {
        m_utilities->debugLogMessage("Printing completed");
        emit printDone();
    } else {
        m_utilities->debugLogMessage("Printing Failed");
        emit printFailed();
    }
}

bool LabelPrinter::writeDataToSocket(QString data)
{
    if (m_connected) {
        m_utilities->debugLogMessage(QString("Writing data: %1").arg(data));

        // Prepend string with "data begin" string
        data.prepend("**********");

        // Append the termination string
        data.append("##########");

        QByteArray ba = data.toLatin1();
        const char *dataStr = ba.data();

        m_socket->write(dataStr, strlen(dataStr));
        if (!m_socket->waitForBytesWritten(5000)) {
            m_utilities->debugLogMessage("ERROR: Failed to write bytes to socket");
            return false;
        } else {
            m_utilities->debugLogMessage("SUCCESS: Bytes written");
            return true;
        }
    } else {
        m_utilities->debugLogMessage("ERROR: Socket not connected, cannot write data to server");
        return false;
    }
}
