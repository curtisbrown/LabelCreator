#include "labelprinter.h"

LabelPrinter::LabelPrinter(QObject *parent, Utilities *utilities)
    : QObject(parent)
    , m_utilities(utilities)
    , m_socket(new QTcpSocket(this))
    , m_hostAddress("172.168.1.200")
    , m_hostPort(9999)
    , m_connected(false)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);

    // Connections
    connect(m_socket, &QTcpSocket::connected, this, [=]() {
        m_utilities->debugLogMessage("Successfully connected to the host");
        m_connected = true;
    });
    connect(m_socket, &QTcpSocket::readyRead, this, [=]() {
        m_utilities->debugLogMessage(m_socket->readAll());
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

    connectToPrinterServer();
}

LabelPrinter::~LabelPrinter()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    m_socket->close();
}

void LabelPrinter::printLabel()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);

    if (writeDataToSocket("INSERT DATA HERE FOR BARTENDER")) {
        m_utilities->debugLogMessage("bytes written successfully");
        emit printDone();
    } else {
        emit printFailed();
    }
}

void LabelPrinter::resetContent()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    connectToPrinterServer();
}

void LabelPrinter::connectToPrinterServer()
{
    m_utilities->debugLogMessage("Attempting to connect to Server");
    m_socket->abort();
    m_socket->connectToHost(m_hostAddress, m_hostPort);
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
