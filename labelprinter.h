#ifndef LABELPRINTER_H
#define LABELPRINTER_H

#include <QObject>
#include <QTcpSocket>
#include <QProcess>
#include <QTimer>

#include "utilities.h"

class LabelPrinter : public QObject
{
    Q_OBJECT
public:
    explicit LabelPrinter(QObject *parent = nullptr, Utilities *utilities = nullptr);
    ~LabelPrinter();

signals:
    void uploadComplete();
    void uploadFail();
    void printFailed();
    void printDone();

public slots:
    void resetContent();
    void connectToPrinterServer();

    bool createFileToUpload(QString serial, QString ssid, QString wifiKey, QString password);
    void startFtpUpload(QString serial, QString ssid, QString wifiKey, QString password);
    void ftpUploadStatus(int exitCode, QProcess::ExitStatus exitStatus);

    void startCheckPrint();
    void checkPrintStatus();
    bool writeDataToSocket(QString data);

private:
    Utilities *m_utilities;
    QTcpSocket *m_socket;
    QString m_hostAddress;
    quint16 m_hostPort;
    bool m_connected;
    QProcess m_curlProcess;
    QProcess m_printProcess;
    QTimer m_timer;
    QString m_host;
    QString m_user;
    QString m_password;
    QString m_serial;
    QString m_dataBuffer;
};

#endif // LABELPRINTER_H
