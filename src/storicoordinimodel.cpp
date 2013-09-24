#include "storicoordinimodel.h"

storicoOrdiniModel::storicoOrdiniModel(QObject *parent) :
  QSqlQueryModel(parent)
{
}

QVariant storicoOrdiniModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) return QVariant();

  QVariant rigaArticolo=QSqlQueryModel::data(index,role);
  switch (index.column()) {
    case 0:
      if(Qt::TextAlignmentRole==role) return Qt::AlignRight|Qt::AlignVCenter;
      break;
    case 1:
      if(Qt::TextAlignmentRole==role) return Qt::AlignRight|Qt::AlignVCenter;
      break;
    case 2:
      break;
    case 3:
      if(Qt::DisplayRole==role) {
        float importo=rigaArticolo.toFloat();
        return QString("%L1").arg(importo,4,'f',2);
      }
      if(Qt::TextAlignmentRole==role) return Qt::AlignRight|Qt::AlignVCenter;
      break;
    default:
      break;
  }
  return rigaArticolo;
}


QVariant storicoOrdiniModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(Qt::Horizontal==orientation && Qt::DisplayRole==role) {
    switch (section) {
      case 0:
        return "SESSIONE";
        break;
      case 1:
        return "NUMERO";
        break;
      case 2:
        return "DATA/ORA";
        break;
      case 3:
        return "IMPORTO";
        break;
      case 4:
        return "STORNO";
        break;
      default:
        break;
    }
  }
  return QSqlQueryModel::headerData(section,orientation,role);
}
