#ifndef ORDINEMODEL_H
#define ORDINEMODEL_H

#include <QAbstractTableModel>

class OrdineModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit OrdineModel(QObject *parent = 0);
  
signals:
  
public slots:

public:
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  bool aggiunge(const QString& valore);
  QVariant data(const QModelIndex &index, int role) const;

private:
  QList<QString> articoloList;
};

#endif // ORDINEMODEL_H
