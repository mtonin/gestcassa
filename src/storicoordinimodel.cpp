#include "commons.h"
#include "storicoordinimodel.h"
#include <QLocale>
#include <QBrush>
#include <QDateTime>
#include <QFont>

storicoOrdiniModel::storicoOrdiniModel(QObject *parent) :
    QSqlTableModel(parent)
{
    setEditStrategy(QSqlTableModel::OnFieldChange);
}

QVariant storicoOrdiniModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    QModelIndex tmpIdx = QSqlTableModel::index(index.row(), 6);
    bool flagStorno = QSqlTableModel::data(tmpIdx).toBool();

    QVariant rigaArticolo = QSqlTableModel::data(index, role);
    if (flagStorno) {
        if (Qt::ForegroundRole == role) {
            QBrush brush(Qt::SolidPattern);
            brush.setColor(Qt::black);
            return QVariant(brush);
        }
        if (Qt::BackgroundRole == role) {
            QBrush brush(Qt::Dense3Pattern);
            brush.setColor(Qt::lightGray);
            return QVariant(brush);
        }
        if(Qt::FontRole==role) {
            QFont fontAttuale(QSqlTableModel::data(index,Qt::FontRole).toString());
            fontAttuale.setStrikeOut(true);
            fontAttuale.setItalic(true);
            return QVariant(fontAttuale);
        }
    }
    switch (index.column()) {
    case 0:
        if (Qt::TextAlignmentRole == role) return (QVariant)(Qt::AlignRight | Qt::AlignVCenter);
        break;
    case 2:
        if (Qt::TextAlignmentRole == role) return (QVariant)(Qt::AlignRight | Qt::AlignVCenter);
        break;
    case 3:
        if (Qt::TextAlignmentRole == role) return (QVariant)(Qt::AlignRight | Qt::AlignVCenter);
        break;
    case 4:
        if (Qt::DisplayRole == role || Qt::EditRole==role) {
            return rigaArticolo.toDateTime().toString("dd/MM/yyyy hh:mm:ss");
        }
        break;
    case 5:
        if (Qt::DisplayRole == role || Qt::EditRole==role) {
            if(rigaArticolo.type()==QMetaType::QString) {
                return rigaArticolo.toString();
            } else {
                float importo = rigaArticolo.toFloat();
                return QString("%1 %L2").arg(QChar(0x20AC)).arg(importo, 4, 'f', 2);
            }
        }
        if (Qt::TextAlignmentRole == role) return (QVariant)(Qt::AlignRight | Qt::AlignVCenter);
        break;
    case 6:
        if (Qt::DisplayRole == role || Qt::EditRole==role) {
            return flagStorno;
        }
        break;
    default:
        break;
    }
    return rigaArticolo;
}


QVariant storicoOrdiniModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role) {
        switch (section) {
        case 0:
            return "SESSIONE";
            break;
        case 2:
            return "CASSA";
            break;
        case 3:
            return "NUMERO";
            break;
        case 4:
            return "DATA/ORA";
            break;
        case 5:
            return "IMPORTO";
            break;
        case 6:
            return "STORNO";
            break;
        default:
            break;
        }
    }
    return QSqlQueryModel::headerData(section, orientation, role);
}

bool storicoOrdiniModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QModelIndex tmpIdx = QSqlTableModel::index(index.row(), 0);
    int sessione = QSqlTableModel::data(tmpIdx).toInt();
    tmpIdx = QSqlTableModel::index(index.row(), 2);
    QString cassa = QSqlTableModel::data(tmpIdx).toString();
    tmpIdx = QSqlTableModel::index(index.row(), 3);
    int numOrdine = QSqlTableModel::data(tmpIdx).toInt();
    if (6 == index.column()) {
        if(Qt::EditRole==role) {
            LOG_INFO("Storno ordine [%d/%s/%04d]: %s",sessione,cassa.toStdString().c_str(),numOrdine,value.toBool()?"STORNATO":"STORNO ANNULLATO");
            QSqlTableModel::setData(index,value);
        }
    }
    return true;
}
