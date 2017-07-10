#include "operazionidlg.h"
#include "ui_operazionidlg.h"
#include <QSignalMapper>

OperazioniDlg::OperazioniDlg(modalitaType modalitaCorrente, QPoint startPoint, QWidget *parent) :
    PopupDialog(parent),
    ui(new Ui::OperazioniDlg)
{
    ui->setupUi(this);

    switch (modalitaCorrente) {
    case CASSA:
        ui->cassaBtn->setDisabled(true);
        break;
    case TEST:
        ui->testBtn->setDisabled(true);
        //ui->gestioneBtn->setDisabled(true);
        //ui->stornoBtn->setDisabled(true);
        break;
    case GESTIONE:
        ui->gestioneBtn->setDisabled(true);
        //ui->testBtn->setDisabled(true);
        break;
    }

    avvia(true,startPoint);
    connect(this,SIGNAL(popupOk()),this,SLOT(init()));
}

OperazioniDlg::~OperazioniDlg()
{
    delete ui;
}

void OperazioniDlg::pulsanteClicked(int idx)
{
    close();
    emit operazioneSelezionata(idx);
}

void OperazioniDlg::init()
{

    QSignalMapper* mapper = new QSignalMapper(this);
    connect(ui->gestioneBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->gestioneBtn, 1);
    connect(ui->cassaBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->cassaBtn, 2);
    connect(ui->testBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->testBtn, 3);
    connect(ui->ricaricaBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->ricaricaBtn, 4);
    connect(ui->buoniBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->buoniBtn, 6);
    connect(ui->aboutBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->aboutBtn, 7);

    connect(mapper, SIGNAL(mapped(int)), this, SLOT(pulsanteClicked(int)));

}
