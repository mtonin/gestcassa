#ifndef buoniQueryModel_H
#define buoniQueryModel_H

#include <QSqlQueryModel>

class buoniQueryModel : public QSqlQueryModel
{
  Q_OBJECT
public:
  explicit buoniQueryModel(QObject *parent = 0);

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);

signals:

public slots:

};

#endif // buoniQueryModel_H
