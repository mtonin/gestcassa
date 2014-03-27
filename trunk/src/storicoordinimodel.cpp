#include "storicoordinimodel.h"
#include <QBrush>
storicoOrdiniModel::storicoOrdiniModel(QObject *parent) :
  QSqlTableModel(parent)
{
  setEditStrategy(QSqlTableModel::OnFieldChange);
}

QVariant storicoOrdiniModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) return QVariant();
  QModelIndex tmpIdx=QSqlTableModel::index(index.row(),4);
  bool flagStorno=QSqlTableModel::data(tmpIdx).toBool();

  QVariant rigaArticolo=QSqlTableModel::data(index,role);
  if(flagStorno) {
    if(Qt::ForegroundRole==role) {
      QBrush brush(Qt::SolidPattern);
      brush.setColor(Qt::black);
      return QVariant(brush);
    }
    if(Qt::BackgroundRole==role) {
      QBrush brush(Qt::Dense3Pattern);
      brush.setColor(Qt::lightGray);
      return QVariant(brush);
    }
  }
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
      if(Qt::DisplayRole==role || Qt::EditRole==role) {
        float importo=rigaArticolo.toFloat();
        return QString("%1 %L2").arg(QChar(0x20AC)).arg(importo,4,'f',2);
      }
      if(Qt::TextAlignmentRole==role) return Qt::AlignRight|Qt::AlignVCenter;
      break;
    case 4:
      if(Qt::DisplayRole==role) {
        return QVariant();
      }
      if(Qt::CheckStateRole==role) {
        return flagStorno?Qt::Checked:Qt::Unchecked;
      }

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

Qt::ItemFlags storicoOrdiniModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags=QAbstractItemModel::flags(index);
  if(1==index.column()) {
    flags |=  Qt::ItemIsEditable;
  }
  if(4==index.column()) {
    flags |=  Qt::ItemIsUserCheckable|Qt::ItemIsEnabled;
  }
  return flags;
}

bool storicoOrdiniModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if(4==index.column()) {
    if(Qt::CheckStateRole==role) {
        QSqlTableModel::setData(index,Qt::Checked==value.toInt()?"true":"false",Qt::EditRole);
        emit dataChanged(index,index);
    }
  }
  return true;
}
