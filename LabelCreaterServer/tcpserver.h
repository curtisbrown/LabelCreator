#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

#include "utilities.h"

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    ~TcpServer();

public slots:
    void newConnectionRequested();
    void generateFile();

private:
    Utilities m_utilities;

    QHostAddress m_hostAddress;
    QString m_address;
    quint16 m_port;
    QString m_filename;
    QString m_fileLocation;
    QString m_socketData;
    QTcpServer *m_server;
    QTcpSocket *m_socket;
};

#endif // TCPSERVER_H
