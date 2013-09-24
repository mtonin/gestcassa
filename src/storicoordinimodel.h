#ifndef STORICOORDINIMODEL_H
#define STORICOORDINIMODEL_H

#include <QSqlQueryModel>

class storicoOrdiniModel : public QSqlQueryModel
{
  Q_OBJECT
public:
  explicit storicoOrdiniModel(QObject *parent = 0);

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

signals:
  
public slots:

};

#endif // STORICOORDINIMODEL_H
