#ifndef STORICOARTICOLIORDINIMODEL_H
#define STORICOARTICOLIORDINIMODEL_H

#include <QSqlQueryModel>

class storicoArticoliOrdiniModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit storicoArticoliOrdiniModel(QObject *parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
signals:

public slots:

};

#endif // STORICOARTICOLIORDINIMODEL_H
