#include "statsmodel.h"

StatsModel::StatsModel(QObject *parent) :
  QStandardItemModel(parent)
{
}

QVariant StatsModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) return QVariant();

  switch (index.column()) {
    case 1:
      if(Qt::TextAlignmentRole==role) return QVariant(Qt::AlignRight|Qt::AlignVCenter);
      break;
    default:
      break;
  }
  return QStandardItemModel::data(index,role);
}

QVariant StatsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(Qt::Horizontal==orientation && Qt::DisplayRole==role) {
    switch (section) {
      case 0:
        return "ARTICOLO";
        break;
      case 1:
        return "QUANTITA'";
        break;
      default:
        break;
    }
  }
  return QStandardItemModel::headerData(section,orientation,role);
}
