#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>
#include <QProcess>
#include <QStandardPaths>

#include "utilities.h"

class ImageProcessing : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessing(QObject *parent = nullptr, Utilities *utilities = nullptr);

signals:

public slots:
    void resetContent();
    void processImageStart();

private:
    Utilities *m_utilities;
    QProcess m_process;
    QString m_homeDir;
    QString m_ocrSwInstallDir;
};

#endif // IMAGEPROCESSING_H
