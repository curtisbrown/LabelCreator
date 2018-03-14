#include "labelprinter.h"

LabelPrinter::LabelPrinter(QObject *parent, Utilities *utilities)
    : QObject(parent)
    , m_utilities(utilities)
    , m_socket(new QTcpSocket(this))
    , m_hostAddress("172.168.1.200")
    , m_hostPort(9999)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);

    // Connections
    connect(m_socket, &QTcpSocket::connected, this, [=]() {
        m_utilities->debugLogMessage("Successfully connected to the host");
    });
    connect(m_socket, &QTcpSocket::readyRead, this, [=]() {
        m_utilities->debugLogMessage(m_socket->readAll());
    });
    connect(m_socket, &QTcpSocket::disconnected, this, [=]() {
        m_utilities->debugLogMessage("Host has disconnected");
    });
    connect(m_socket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), this, [=]() {
        m_utilities->debugLogMessage("Socket error, killing connection to host");
        m_socket->disconnectFromHost();
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
    m_utilities->debugLogMessage("Printing label");

    m_socket->write("**********", strlen("**********"));

    m_socket->write("INSERT DATA HERE FOR BARTENDER", strlen("INSERT DATA HERE FOR BARTENDER"));

    m_socket->write("##########", strlen("##########"));

    if (m_socket->waitForBytesWritten(10000)) {
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
