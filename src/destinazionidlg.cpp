#include "destinazionidlg.h"

#include <QMessageBox>
#include <QSqlRecord>

DestinazioniDlg::DestinazioniDlg(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    modello = new QSqlTableModel;
    modello->setEditStrategy(QSqlTableModel::OnFieldChange);
    modello->setTable("destinazionistampa");
    //modello->setSort(0,Qt::AscendingOrder);
    modello->select();
    destinazioneTbl->setModel(modello);
    destinazioneTbl->hideColumn(1);
    destinazioneTbl->hideColumn(2);
    destinazioneTbl->hideColumn(3);
    destinazioneTbl->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);

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
        QMessageBox::critical(this, "Errore", "Impossibile cancellare la destinazione. Controllare gli articoli in cui � impostata.");
        return;
    }
    currRiga--;
    if(currRiga<0) currRiga=0;
    destinazioneTbl->setCurrentIndex(modello->index(currRiga, 0));
    destinazioneTbl->setFocus();
}

void DestinazioniDlg::on_nuovoBtn_clicked()
{
    int numRighe = modello->rowCount();
    modello->insertRow(numRighe);
    modello->setData(modello->index(numRighe, 0), "NOME DESTINAZIONE");
    modello->setData(modello->index(numRighe, 1), "INTESTAZIONE TAGLIANDO");
    modello->setData(modello->index(numRighe, 2), true);
    modello->setData(modello->index(numRighe, 3), false);
    modello->submitAll();
    destinazioneTbl->setCurrentIndex(modello->index(numRighe, 0));
    destinazioneTxt->selectAll();
    destinazioneTxt->setFocus();
}
