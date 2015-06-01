#include "buonimodel.h"

buoniModel::buoniModel(QObject *parent) :
QSqlTableModel(parent)
{
}

QVariant buoniModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    QModelIndex tmpIdx = QSqlTableModel::index(index.row(), 5);
    bool flagStorno = QSqlTableModel::data(tmpIdx).toBool();

    QVariant rigaArticolo = QSqlTableModel::data(index, role);
    switch (index.column()) {
    case 0:
        if (Qt::TextAlignmentRole == role) return Qt::AlignRight | Qt::AlignVCenter;
        break;
    case 1:
        if (Qt::TextAlignmentRole == role) return Qt::AlignRight | Qt::AlignVCenter;
        break;
    case 2:
        if (Qt::TextAlignmentRole == role) return Qt::AlignRight | Qt::AlignVCenter;
        break;
    case 3:
        break;
    case 4:
        if (Qt::DisplayRole == role || Qt::EditRole == role) {
            float importo = rigaArticolo.toFloat();
            return QString("%1 %L2").arg(QChar(0x20AC)).arg(importo, 4, 'f', 2);
        }
        if (Qt::TextAlignmentRole == role) return Qt::AlignRight | Qt::AlignVCenter;
        break;
    case 5:
        if (Qt::DisplayRole == role) {
            return QVariant();
        }
        if (Qt::CheckStateRole == role) {
            return flagStorno ? Qt::Checked : Qt::Unchecked;
        }

    default:
        break;
    }
    return rigaArticolo;
}


QVariant buoniModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role) {
        switch (section) {
        case 0:
            return "NUMERO";
            break;
        case 1:
            return "NOMINATIVO";
            break;
        case 2:
            return "DATA/ORA";
            break;
        default:
            break;
        }
    }
    return QSqlQueryModel::headerData(section, orientation, role);
}

Qt::ItemFlags buoniModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (2 == index.column()) {
        flags |=  Qt::ItemIsEditable;
    }
    return flags;
}

bool buoniModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return true;
}
