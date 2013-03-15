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
  return 1;
}

bool OrdineModel::aggiunge(const QString& valore)
{
  beginInsertRows(QModelIndex(),articoloList.count(),articoloList.count());
  articoloList.append(valore);
  endInsertRows();
  return true;
}

QVariant OrdineModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid()) return QVariant();
  return articoloList.at(index.row());
}
