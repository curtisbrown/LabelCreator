#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>

#include "utilities.h"

class ImageProcessing : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessing(QObject *parent = nullptr, Utilities *utilities = nullptr);

signals:

public slots:
    void resetContent();

private:
    Utilities *m_utilities;
};

#endif // IMAGEPROCESSING_H
