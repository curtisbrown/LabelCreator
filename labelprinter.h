#ifndef LABELPRINTER_H
#define LABELPRINTER_H

#include <QObject>

#include "utilities.h"

class LabelPrinter : public QObject
{
    Q_OBJECT
public:
    explicit LabelPrinter(QObject *parent = nullptr, Utilities *utilities = nullptr);

signals:
    void printDone();

public slots:
    void printLabel();
    void resetContent();

private:
    Utilities *m_utilities;
};

#endif // LABELPRINTER_H
