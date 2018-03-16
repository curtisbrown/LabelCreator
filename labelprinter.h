#ifndef LABELPRINTER_H
#define LABELPRINTER_H

#include <QObject>
#include <QTcpSocket>

#include "utilities.h"

class LabelPrinter : public QObject
{
    Q_OBJECT
public:
    explicit LabelPrinter(QObject *parent = nullptr, Utilities *utilities = nullptr);
    ~LabelPrinter();

signals:
    void printFailed();
    void printDone();

public slots:
    void printLabel();
    void resetContent();
    void connectToPrinterServer();

    bool writeDataToSocket(QString data);

private:
    Utilities *m_utilities;
    QTcpSocket *m_socket;
    QString m_hostAddress;
    quint16 m_hostPort;
    bool m_connected;
};

#endif // LABELPRINTER_H
