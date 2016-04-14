#include "destinazionidlg.h"
#include "confermadlg.h"

#include <QMessageBox>
#include <QSqlRecord>
#include <QtSql>

DestinazioniDlg::DestinazioniDlg(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    modello = new QSqlTableModel;
    modello->setEditStrategy(QSqlTableModel::OnFieldChange);
    modello->setTable("destinazionistampa");
    modello->setSort(0,Qt::AscendingOrder);
    modello->select();
    destinazioneTbl->setModel(modello);
    destinazioneTbl->hideColumn(1);
    destinazioneTbl->hideColumn(2);
    destinazioneTbl->hideColumn(3);
    destinazioneTbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    mapper = new QDataWidgetMapper;
    mapper->setModel(modello);
    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->addMapping(destinazioneTxt, 0);
    mapper->addMapping(intestazioneTxt, 1);
    mapper->addMapping(stampaFlagBox, 2);
    mapper->addMapping(stampaNumeroRitiro, 3);
    mapper->toFirst();

    connect(destinazioneTbl->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), mapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(modello, SIGNAL(dataChanged(QModelIndex, QModelIndex)), mapper, SLOT(setCurrentModelIndex(QModelIndex)));

    destinazioneTbl->setCurrentIndex(modello->index(0, 0));
    destinazioneTxt->setFocus();
}

DestinazioniDlg::~DestinazioniDlg()
{
    delete modello;
    delete mapper;
}

void DestinazioniDlg::on_cancellaBtn_clicked()
{
    int currRiga = destinazioneTbl->currentIndex().row();
    if (!modello->removeRow(currRiga)) {
        QMessageBox::critical(this, "Errore", "Impossibile cancellare la destinazione. Controllare gli articoli in cui è impostata.");
        return;
    }
    currRiga--;
    if (currRiga < 0) currRiga = 0;
    destinazioneTbl->setCurrentIndex(modello->index(currRiga, 0));
    destinazioneTbl->setFocus();
}

void DestinazioniDlg::on_nuovoBtn_clicked()
{
    ConfermaDlg dlg("Inserire il nome della nuova destinazione.", "Nome", false,this);
    if (QDialog::Accepted != dlg.visualizza()) return;
    QString nuovaDestinazione=dlg.getValore();

    QString query("insert into destinazionistampa values (?,'INTESTAZIONE TAGLIANDO',1,0)");
    QSqlQuery sql;
    if(!sql.prepare(query)) {
      QSqlError errore=sql.lastError();
      QString msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.databaseText());
      QMessageBox::critical(this,"Errore",msg);
      return;
    }
    sql.addBindValue(nuovaDestinazione);
    if(!sql.exec()) {
      QSqlError errore=sql.lastError();
      QString msg;
      if(-803==errore.number()) {
        msg="Destinazione già presente.";
      } else {
        msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.text());
      }
      QMessageBox::critical(this,"Errore",msg);
    } else {
      modello->select();
    }
    destinazioneTbl->setCurrentIndex(modello->index(0, 0));
    destinazioneTxt->setFocus();
    /*
    int numRighe = modello->rowCount();
    modello->insertRow(numRighe);
    modello->setData(modello->index(numRighe, 0), nuovaDestinazione);
    modello->setData(modello->index(numRighe, 1), "INTESTAZIONE TAGLIANDO");
    modello->setData(modello->index(numRighe, 2), true);
    modello->setData(modello->index(numRighe, 3), false);
    if(!modello->submitAll()) {
      QSqlError errore=modello->lastError();
      QString msg;
      if(-803==errore.number()) {
        msg="Destinazione già presente.";
      } else {
        msg=QString("Errore codice=%1,descrizione=%2").arg(errore.number()).arg(errore.text());
      }
      QMessageBox::critical(this,"Errore",msg);
      modello->removeRow(numRighe);
    } else {
      destinazioneTbl->setCurrentIndex(modello->index(numRighe, 0));
      destinazioneTxt->selectAll();
      destinazioneTxt->setFocus();
    }
    */
}
