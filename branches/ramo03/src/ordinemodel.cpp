#include "commons.h"
#include "ordinemodel.h"
#include <QtSql>
#include <QMessageBox>

OrdineModel::OrdineModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

void OrdineModel::incrementa(const int id)
{
    incrementa(id, "", 0);
}

void OrdineModel::decrementa(const int id, const QModelIndex& currIndex)
{
    QMutableListIterator<rigaArticoloClass> it(articoloList);
    while (it.hasNext()) {
        rigaArticoloClass rigaArticolo = it.next();
        if (rigaArticolo.id == id) {
            rigaArticolo.quantita--;
            if (rigaArticolo.quantita > 0) {
                it.setValue(rigaArticolo);
            } else {
                beginRemoveRows(QModelIndex(), currIndex.row(), currIndex.row());
                it.remove();
                endRemoveRows();
                emit rigaCancellata();
            }
            emit dataChanged(QModelIndex(), QModelIndex());
            return;
        }
    }

}

int OrdineModel::rowCount(const QModelIndex &parent) const
{
    return articoloList.count();
}

int OrdineModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

bool OrdineModel::incrementa(const int id, const QString& descrizione, const float prezzo)
{
    QMutableListIterator<rigaArticoloClass> it(articoloList);
    while (it.hasNext()) {
        rigaArticoloClass rigaArticolo = it.next();
        if (rigaArticolo.id == id) {
            rigaArticolo.quantita++;
            it.setValue(rigaArticolo);
            emit dataChanged(QModelIndex(), QModelIndex());
            return true;
        }
    }

    beginInsertRows(QModelIndex(), articoloList.count(), articoloList.count());
    rigaArticoloClass rigaArticolo;
    rigaArticolo.id = id;
    rigaArticolo.quantita = 1;
    rigaArticolo.descrizione = descrizione;
    rigaArticolo.prezzo = prezzo;
    articoloList.append(rigaArticolo);
    endInsertRows();

    emit dataChanged(QModelIndex(), QModelIndex());
    return true;
}

void OrdineModel::clear()
{
    articoloList.clear();
    reset();
    emit dataChanged(QModelIndex(), QModelIndex());
}

bool OrdineModel::completaOrdine(const int numeroOrdine, const float importo, const int idSessione, QString idCassa)
{
    QSqlDatabase db(QSqlDatabase::database());
    db.transaction();

    QSqlQuery stmt;
    if (!stmt.exec("delete from ordinirighe") ||
        !stmt.exec("delete from ordini")) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              stmt.lastError().text());
        db.rollback();
        return false;
    }

    stmt.prepare("insert into ordini(numero,tsstampa,importo) values(?,?,?)");
    stmt.addBindValue(numeroOrdine);
    QDateTime ts = QDateTime::currentDateTime();
    stmt.addBindValue(ts.toString("yyyy-MM-dd hh:mm:ss"));
    stmt.addBindValue(importo);
    if (!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              stmt.lastError().text());
        db.rollback();
        return false;
    }

    foreach(rigaArticoloClass rigaArticolo, articoloList) {
        stmt.prepare("insert into ordinirighe(numeroordine,idarticolo,quantita) values(?,?,?)");
        stmt.addBindValue(numeroOrdine);
        stmt.addBindValue(rigaArticolo.id);
        stmt.addBindValue(rigaArticolo.quantita);
        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"),
                                  stmt.lastError().text());
            db.rollback();
            return false;
        }
    }

    if (idSessione != ID_SESSIONE_TEST) {
        stmt.prepare("insert into storicoordinitot values(?,?,?,?,?,0)");
        stmt.addBindValue(idSessione);
        stmt.addBindValue(idCassa);
        stmt.addBindValue(numeroOrdine);
        stmt.addBindValue(ts.toString("yyyy-MM-dd hh:mm:ss"));
        stmt.addBindValue(importo);
        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"),
                                  stmt.lastError().text());
            db.rollback();
            return false;
        }

        stmt.prepare("insert into storicoordinidett select ?,?,numeroordine,descrizione,quantita,destinazione,prezzo,tipoArticolo from dettagliordine where numeroordine=?");
        stmt.addBindValue(idSessione);
        stmt.addBindValue(idCassa);
        stmt.addBindValue(numeroOrdine);
        if (!stmt.exec()) {
            QMessageBox::critical(0, QObject::tr("Database Error"),
                                  stmt.lastError().text());
            db.rollback();
            return false;
        }
    }

    db.commit();
    return true;
}

QVariant OrdineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    rigaArticoloClass rigaArticolo = articoloList.value(index.row());
    float totRiga;
    switch (index.column()) {
    case 0:
        if (Qt::DisplayRole == role) return rigaArticolo.id;
        break;
    case 1:
        if (Qt::DisplayRole == role) return rigaArticolo.descrizione;
        if (Qt::TextAlignmentRole == role) return Qt::AlignLeft | Qt::AlignVCenter;
        if (Qt::ToolTipRole == role) return rigaArticolo.descrizione;
        break;
    case 2:
        if (Qt::DisplayRole == role) return rigaArticolo.quantita;
        if (Qt::TextAlignmentRole == role) return Qt::AlignRight | Qt::AlignVCenter;
        break;
    case 3:
        if (Qt::DisplayRole == role) {
            totRiga = rigaArticolo.quantita * rigaArticolo.prezzo;
            return QString("%L1").arg(totRiga, 4, 'f', 2);
        }
        if (Qt::UserRole == role) {
            return totRiga = rigaArticolo.quantita * rigaArticolo.prezzo;
        }
        if (Qt::TextAlignmentRole == role) return Qt::AlignRight | Qt::AlignVCenter;
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant OrdineModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role) {
        switch (section) {
        case 0:
            return "ID";
            break;
        case 1:
            return "ARTICOLO";
            break;
        case 2:
            return "Q.TA'";
            break;
        case 3:
            return "PARZIALE";
            break;
        default:
            break;
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}