#ifndef ORDINEMODEL_H
#define ORDINEMODEL_H

#include <QAbstractTableModel>

class OrdineModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit OrdineModel(QObject *parent = 0);
  
signals:
  void rigaCancellata();

public slots:
  void incrementa(const int id);
  void decrementa(const int id);

public:
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  bool incrementa(const int id, const QString& descrizione,const float prezzo);
  void clear();

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
