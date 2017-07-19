#ifndef STORICOORDINIMODEL_H
#define STORICOORDINIMODEL_H

#include <QSqlTableModel>

class storicoOrdiniModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit storicoOrdiniModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

public slots:

};

#endif // STORICOORDINIMODEL_H
