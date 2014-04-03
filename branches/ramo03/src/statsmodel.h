#ifndef STATSMODEL_H
#define STATSMODEL_H

#include <QStandardItemModel>

class StatsModel : public QStandardItemModel
{
  Q_OBJECT
public:
  explicit StatsModel(QObject *parent = 0);
  
public:
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

};

#endif // STATSMODEL_H
