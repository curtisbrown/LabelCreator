#include "tcpserver.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>

TcpServer::TcpServer(QObject *parent)
    : QObject(parent)
    , m_utilities(this)
    , m_filename("testOutput.txt")
    , m_fileLocation(QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory))
    , m_socketData("")
    , m_server(new QTcpServer(this))
    , m_socket(new QTcpSocket(this))
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);

    // Set address of Server here
    m_hostAddress.setAddress("172.168.1.200");

    // Whenever a user connects, it will emit signal
    connect(m_server, &QTcpServer::newConnection, this, [=]() {
        m_utilities.debugLogMessage("New Connection request");
        newConnectionRequested();
    });

    if (!m_server->listen(m_hostAddress, 9999))
        m_utilities.debugLogMessage("Server could not start");
    else
        m_utilities.debugLogMessage("Server started!");
}

TcpServer::~TcpServer()
{
    m_utilities.debugLogMessage("Closing Socket");
    m_socket->close();
}

void TcpServer::newConnectionRequested()
{
    m_socket = m_server->nextPendingConnection();

    // Connection needs to be made here since m_socket changes each time
    connect(m_socket, &QTcpSocket::readyRead, this, [=]() {
        m_utilities.debugLogMessage("Recieving data");
        generateFile();
    });

    m_socket->write("Connection Confirm\r\n");
    if (m_socket->waitForBytesWritten(3000))
        m_utilities.debugLogMessage("Bytes written successfully");
    else
        m_utilities.debugLogMessage("ERROR: could not write bytes");
}

void TcpServer::generateFile()
{
    m_utilities.debugLogMessage(Q_FUNC_INFO);

    m_socketData += m_socket->readAll();
    if (m_socketData.startsWith("**********") && m_socketData.endsWith("##########")) {
        // Remove header and termination strings
        m_socketData.remove("**********");
        m_socketData.remove("##########");
        m_utilities.debugLogMessage( "Writing content to file");
        QFile bartenderFile(m_fileLocation + m_filename);
        if (bartenderFile.open(QIODevice::WriteOnly)) {
            QTextStream in(&bartenderFile);
            in << m_socketData;

            bartenderFile.close();
            m_socketData.clear();
            m_utilities.debugLogMessage( "DONE!");
        } else {
            m_utilities.debugLogMessage("ERROR opening file");
        }
    }
}
