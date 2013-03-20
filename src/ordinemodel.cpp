#include "ordinemodel.h"

OrdineModel::OrdineModel(QObject *parent) :
  QAbstractTableModel(parent)
{
}

int OrdineModel::rowCount(const QModelIndex &parent) const
{
  return articoloList.count();
}

int OrdineModel::columnCount(const QModelIndex &parent) const
{
  return 4;
}

bool OrdineModel::aggiunge(const int id,const QString& descrizione,const float prezzo)
{
  QMutableListIterator<rigaArticoloClass> it(articoloList);
  while(it.hasNext()) {
    rigaArticoloClass rigaArticolo=it.next();
    if(rigaArticolo.id==id) {
      rigaArticolo.quantita++;
      it.setValue(rigaArticolo);
      emit dataChanged(QModelIndex(),QModelIndex());
      return true;
    }
  }

  beginInsertRows(QModelIndex(),articoloList.count(),articoloList.count());
  rigaArticoloClass rigaArticolo;
  rigaArticolo.id=id;
  rigaArticolo.quantita=1;
  rigaArticolo.descrizione=descrizione;
  rigaArticolo.prezzo=prezzo;
  articoloList.append(rigaArticolo);
  endInsertRows();

  return true;
}

QVariant OrdineModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) return QVariant();
  if(role!=Qt::DisplayRole) {
    return QVariant();
  }
  rigaArticoloClass rigaArticolo=articoloList.value(index.row());
  float totRiga;
  switch (index.column()) {
    case 1:
      return rigaArticolo.descrizione;
      break;
    case 2:
      return rigaArticolo.quantita;
      break;
    case 3:
      totRiga=rigaArticolo.quantita*rigaArticolo.prezzo;
      return QString("%1").arg(totRiga,4,'f',2);
      break;
    default:
      break;
  }
  return QVariant();
}

QVariant OrdineModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(Qt::Horizontal==orientation && Qt::DisplayRole==role) {
    switch (section) {
      case 0:
        return "ID";
        break;
      case 1:
        return "ARTICOLO";
        break;
      case 2:
        return "Q.TA'";
        break;
      case 3:
        return "PARZIALE";
        break;
      default:
        break;
    }
  }
  return QAbstractItemModel::headerData(section,orientation,role);
}
