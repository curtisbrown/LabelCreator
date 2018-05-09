#include "imageprocessing.h"

ImageProcessing::ImageProcessing(QObject *parent, Utilities *utilities) :
    QObject(parent),
    m_utilities(utilities)
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
}
