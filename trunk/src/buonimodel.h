#ifndef BUONIMODEL_H
#define BUONIMODEL_H

#include <QSqlTableModel>

class buoniModel : public QSqlTableModel
{
  Q_OBJECT
public:
  explicit buoniModel(QObject *parent = 0);

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);

signals:

public slots:

};

#endif // BUONIMODEL_H
