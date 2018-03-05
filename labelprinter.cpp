#include "labelprinter.h"

LabelPrinter::LabelPrinter(QObject *parent, Utilities *utilities) :
    QObject(parent),
    m_utilities(utilities)
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
}

void LabelPrinter::printLabel()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
    m_utilities->debugLogMessage("Printing label");
}

void LabelPrinter::resetContent()
{
    m_utilities->debugLogMessage(Q_FUNC_INFO);
}
