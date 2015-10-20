#include <QtSql>
#include <QMessageBox>
#include "buoniquerymodel.h"

buoniQueryModel::buoniQueryModel(QObject *parent) :QSqlQueryModel(parent)
{
}

QVariant buoniQueryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    QModelIndex tmpIdx = QSqlQueryModel::index(index.row(), 2);
    bool flagAnnullato = QSqlQueryModel::data(tmpIdx).toBool();
    if (flagAnnullato) {
        if (Qt::ForegroundRole == role) {
            QBrush brush(Qt::SolidPattern);
            brush.setColor(Qt::black);
            return QVariant(brush);
        }
        if (Qt::BackgroundRole == role) {
            QBrush brush(Qt::Dense3Pattern);
            brush.setColor(Qt::lightGray);
            return QVariant(brush);
        }
    }

    QVariant rigaArticolo = QSqlQueryModel::data(index, role);
    switch (index.column()) {
    case 0:
        if (Qt::TextAlignmentRole == role) return Qt::AlignLeft | Qt::AlignVCenter;
        break;
    case 1:
        if (Qt::TextAlignmentRole == role) return Qt::AlignLeft | Qt::AlignVCenter;
        break;
    case 2:
        if (Qt::TextAlignmentRole == role) return Qt::AlignCenter;
        if (Qt::DisplayRole == role) {
            return QVariant();
        }
        if (Qt::CheckStateRole == role) {
            return flagAnnullato ? Qt::Checked : Qt::Unchecked;
        }

    default:
        break;
    }
    return rigaArticolo;
}


QVariant buoniQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role) {
        switch (section) {
        case 0:
            return "INTESTAZIONE";
            break;
        case 1:
            return "DATA/ORA EMISSIONE";
            break;
        case 2:
            return "ANNULLATO";
            break;
        default:
            break;
        }
    }
    return QSqlQueryModel::headerData(section, orientation, role);
}

Qt::ItemFlags buoniQueryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (2 == index.column()) {
        flags |=  Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    }
    return flags;
}

bool buoniQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (2 == index.column()) {
      if (Qt::CheckStateRole == role) {
        QString cognome=QSqlQueryModel::index(index.row(),3).data().toString();
        QString nome=QSqlQueryModel::index(index.row(),4).data().toString();
        int flagAnnullato=Qt::Checked == value.toInt() ? 1 : 0;
        QString sql("update buoni set flagannullato=? where cognome=? and nome=?");
        QSqlQuery query;
        if(!query.prepare(sql)) {
          QSqlError errore=query.lastError();
          QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
          QMessageBox::critical(0,"Errore",msg);
          return false;
        }
        query.addBindValue(flagAnnullato);
        query.addBindValue(cognome);
        query.addBindValue(nome);
        if(!query.exec()) {
          QMessageBox::critical(0, QObject::tr("Database Error"),query.lastError().text());
          return false;
        }
          //QSqlTableModel::setData(index, Qt::Checked == value.toInt() ? "1" : "0", Qt::EditRole);
          emit dataChanged(index, index);
      }
  }
  return true;
}
