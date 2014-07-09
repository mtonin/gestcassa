#include "storicoarticoliordinimodel.h"

storicoArticoliOrdiniModel::storicoArticoliOrdiniModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

QVariant storicoArticoliOrdiniModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    switch (index.column()) {
    case 0:
        if (Qt::TextAlignmentRole == role) return Qt::AlignRight | Qt::AlignVCenter;
        break;
    case 1:
        if (Qt::DisplayRole == role) {
            QString descrizione = QSqlQueryModel::data(index, role).toString().simplified();
            return QVariant(descrizione);
        }
        break;
    default:
        break;
    }
    return QSqlQueryModel::data(index, role);
}


QVariant storicoArticoliOrdiniModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role) {
        switch (section) {
        case 0:
            return "Q.TA'";
            break;
        case 1:
            return "ARTICOLO";
            break;
        default:
            break;
        }
    }
    return QSqlQueryModel::headerData(section, orientation, role);
}
