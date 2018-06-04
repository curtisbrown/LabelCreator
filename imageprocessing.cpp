#include "imageprocessing.h"

ImageProcessing::ImageProcessing(QObject *parent, Utilities *utilities) :
    QObject(parent),
    m_utilities(utilities),
    m_homeDir(QStandardPaths::StandardLocation(QStandardPaths::HomeLocation)),
    m_ocrSwInstallDir(QStandardPaths::StandardLocation(QStandardPaths::HomeLocation) + "/OCR/ocrpy-master")
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
}

void ImageProcessing::resetContent()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
}

void ImageProcessing::processImageStart()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    m_process.start("python universal.py");
    m_process.waitForFinished(5000);
}
