#include "operazionidlg.h"
#include "ui_operazionidlg.h"
#include <QDesktopWidget>
#include <QSignalMapper>

OperazioniDlg::OperazioniDlg(modalitaType modalitaCorrente, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OperazioniDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);
    activateWindow();

    switch (modalitaCorrente) {
    case CASSA:
        ui->cassaBtn->setDisabled(true);
        ui->configurazioneBtn->setDisabled(true);
        ui->reportBtn->setDisabled(true);
        break;
    case TEST:
        ui->testBtn->setDisabled(true);
        ui->gestioneBtn->setDisabled(true);
        ui->configurazioneBtn->setDisabled(true);
        ui->reportBtn->setDisabled(true);
        ui->stornoBtn->setDisabled(true);
        break;
    case GESTIONE:
        ui->gestioneBtn->setDisabled(true);
        ui->testBtn->setDisabled(true);
        break;
    }

    QPoint pos = QCursor::pos();
    pos.setY(pos.y() - height());
    //QDesktopWidget* desktop=QApplication::desktop();
    //int screenHeight=desktop->screenGeometry().height();
    if (pos.y() < 0) {
        pos.setY(0);
    }
    move(pos);

    QSignalMapper* mapper = new QSignalMapper(this);
    connect(ui->closeBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->closeBtn, 1);
    connect(ui->reportBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->reportBtn, 2);
    connect(ui->configurazioneBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->configurazioneBtn, 3);
    connect(ui->gestioneBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->gestioneBtn, 4);
    connect(ui->cassaBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->cassaBtn, 5);
    connect(ui->testBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->testBtn, 6);
    connect(ui->statsBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->statsBtn, 7);
    connect(ui->stornoBtn, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(ui->stornoBtn, 8);

    connect(mapper, SIGNAL(mapped(int)), this, SLOT(pulsanteClicked(int)));

}

OperazioniDlg::~OperazioniDlg()
{
    delete ui;
}

void OperazioniDlg::pulsanteClicked(int idx)
{
    close();
    qDebug(QString("Operazione: %1\n").arg(idx).toAscii());
    emit operazioneSelezionata(idx);
}
