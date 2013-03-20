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
  bool aggiunge(const int id, const QString& descrizione,const float prezzo);
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
  class rigaArticoloClass {
  public:
    int id;
    QString descrizione;
    int quantita;
    float prezzo;
  } rigaArticolo;

  QList<rigaArticoloClass> articoloList;
};

#endif // ORDINEMODEL_H
