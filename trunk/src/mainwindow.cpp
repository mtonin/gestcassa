#include "svnrev.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "repartobtnwidget.h"
#include "articolobtnwidget.h"
#include "dettaglireparto.h"
#include "configurazionedlg.h"
#include "reportform.h"
#include "infowidget.h"
#include "QDigitalClock.h"
#include "statsform.h"
#include "dbdialog.h"

#include <QtGui>
#include <QMessageBox>
#include <QHBoxLayout>

const int NUM_REPARTI=8;
const int NUM_RIGHE_ART=5;
const int NUM_COLONNE_ART=6;

MainWindow::MainWindow(QMap<QString,QVariant>* configurazione,QWidget *parent) : confMap(configurazione),QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  dettagliRepartoBox=new DettagliReparto;
  dettagliArticoloBox=new DettagliArticolo;
  ordineBox=new Ordine(confMap);

  ui->latoStackedWidget->addWidget(new QFrame);
  ui->latoStackedWidget->addWidget(dettagliRepartoBox);
  ui->latoStackedWidget->addWidget(dettagliArticoloBox);
  ui->latoStackedWidget->addWidget(ordineBox);

  connect(this,SIGNAL(aggiungeArticolo(int,QString,float)),ordineBox,SLOT(nuovoArticolo(int,QString,float)));

  QDigitalClock* orologio=new QDigitalClock;
  orologio->SetFormat("dd/MM/yyyy\nHH:mm:ss");
  QFont font=orologio->font();
  font.setBold(true);
  font.setPointSize(10);
  orologio->setFont(font);
  //orologio->SetTextColor(Qt::red);
  orologio->SetAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

  QHBoxLayout* clockLayout=new QHBoxLayout;
  clockLayout->setContentsMargins(QMargins(0,0,0,0));
  clockLayout->addWidget(orologio);
  ui->clockFrame->setLayout(clockLayout);

  QStringList messaggi=QString("GESTIONE CASSA,build %1").arg(SVN_REV.c_str()).split(",");
  QString descrizione=confMap->value("descrManifestazione").toString();
  if(!descrizione.isEmpty()) {
    messaggi.insert(0,descrizione);
  }
  info=new infoWidget(messaggi);

  QHBoxLayout* infoLayout=new QHBoxLayout;
  infoLayout->setContentsMargins(QMargins(1,1,1,1));
  infoLayout->addWidget(info);
  ui->infoFrame->setLayout(infoLayout);

  creaRepartiButtons();

  gestioneModalita(CASSA);

  setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
  showMaximized();

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::gestioneModalita(const modalitaType nuovaModalita)
{
  if(GESTIONE==nuovaModalita) {
    DBDialog* dlg=new DBDialog(confMap);
    if(QDialog::Accepted!=dlg->exec()) return;
    ui->configurazioneBtn->setEnabled(true);
    ui->reportBtn->setEnabled(true);
    ui->cassaBtn->setEnabled(true);
    ui->gestioneBtn->setEnabled(false);
    ui->latoStackedWidget->setCurrentIndex(0);

    // attiva tutti i pulsanti dei reparti
    QListIterator<RepartoBtnWidget*> itReparti(repartiList);
    while(itReparti.hasNext()) {
      //itReparti.next()->setVisible(true);
      itReparti.next()->setEnabled(true);
    }
    QListIterator<QStackedWidget*> it(pulsantiList);
    while(it.hasNext()) {
      it.next()->setCurrentIndex(0);
    }
  } else {
    ui->configurazioneBtn->setEnabled(false);
    ui->reportBtn->setEnabled(false);
    ui->gestioneBtn->setEnabled(false);
    ui->cassaBtn->setEnabled(false);
    ui->gestioneBtn->setEnabled(true);

    //ui->modalitaBtn->setText("GESTIONE");
    //ui->modalitaBtn->setIcon(QIcon(":/GestCassa/gestione"));
    ui->latoStackedWidget->setCurrentWidget(ordineBox);

    int primoRepartoAttivo=-1;

    // nasconde i pulsanti dei reparti disabilitati
    QListIterator<RepartoBtnWidget*> itReparti(repartiList);
    while(itReparti.hasNext()) {
      RepartoBtnWidget* reparto=itReparti.next();
      //reparto->setVisible(reparto->getAbilitato());
      reparto->setEnabled(reparto->getAbilitato());
      if(primoRepartoAttivo<0 && reparto->getAbilitato()) primoRepartoAttivo=reparto->getId()-1;
    }

    // nasconde i pulsanti degli articoli disabilitati
    QListIterator<QStackedWidget*> it(pulsantiList);
    while(it.hasNext()) {
      QStackedWidget* box=it.next();
      ArticoloBtnWidget* btnWidget=(ArticoloBtnWidget*)box->widget(0);
      if(!btnWidget->getAbilitato() || btnWidget->getNomeArticolo().isEmpty()) {
        box->setCurrentIndex(1);
      }
    }

    ui->articoliStackedWidget->setCurrentIndex(primoRepartoAttivo);
    showMaximized();
  }

  modalitaCorrente=nuovaModalita;
}

void MainWindow::keyPressEvent(QKeyEvent *evt) {
  switch(evt->key()) {
    case Qt::Key_F11: {
        if(isMaximized()) {
          //setWindowFlags(Qt::Window);
          showNormal();
        } else {
          //setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
          showMaximized();
        }
      }
  }
}

void MainWindow::closeEvent(QCloseEvent *evt)
{
  if(QMessageBox::Ok!=QMessageBox::question(0,"Uscita","Confermi l'uscita?",QMessageBox::Ok|QMessageBox::No)) {
    evt->ignore();
  }
}

void MainWindow::creaRepartiButtons(){

  pulsantiList.clear();
  QLayout* hboxLayout = ui->repartiBox->layout();
  if(hboxLayout) delete hboxLayout;

  while(ui->articoliStackedWidget->count()>0) {
    QWidget* w=ui->articoliStackedWidget->widget(0);
    ui->articoliStackedWidget->removeWidget(w);
  }

  hboxLayout = new QHBoxLayout(ui->repartiBox);
  hboxLayout->setSpacing(2);
  hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
  hboxLayout->setContentsMargins(-1, 5, -1, 5);
  for(int i=1;i<=NUM_REPARTI;i++) {
    RepartoBtnWidget* reparto01Btn = new RepartoBtnWidget(i,ui->repartiBox);

    repartiList.append(reparto01Btn);
    hboxLayout->addWidget(reparto01Btn);
    creaArticoliPerRepartoButtons(i,reparto01Btn);

    connect(reparto01Btn,SIGNAL(clicked()),this,SLOT(repartoSelezionato()));
  }

  ui->repartiBox->setLayout(hboxLayout);
  ui->latoStackedWidget->setCurrentIndex(0);

}

void MainWindow::creaArticoliPerRepartoButtons(int numReparto,RepartoBtnWidget* repartoBtn)   {

  QColor coloreSfondo=repartoBtn->buttonColorNormal();
  QColor coloreCarattere=repartoBtn->textColorEnabled();
  QFont currentFont=repartoBtn->getFont();
  QGridLayout* griglia=new QGridLayout;
  griglia->setSpacing(2);

  QFrame* pagina=new QFrame;
  ui->articoliStackedWidget->addWidget(pagina);
  pagina->setLayout(griglia);

  bool visualizzaPrezzo=confMap->value("visualizzazionePrezzo").toBool();
  for(int riga=0;riga<NUM_RIGHE_ART;riga++) {
    for(int col=0;col<NUM_COLONNE_ART;col++) {
      QStackedWidget* stackedBox=new QStackedWidget();

      ArticoloBtnWidget* btn=new ArticoloBtnWidget(repartoBtn->getId(),riga,col);
      btn->SetButtonColorNormal(coloreSfondo);
      btn->SetButtonColorHot(coloreSfondo);
      btn->SetTextColorEnabled(coloreCarattere);
      btn->setFont(currentFont);
      btn->setVisualizzaPrezzo(visualizzaPrezzo);
      stackedBox->addWidget(btn);
      QFrame* blankFrame=new QFrame;
      stackedBox->addWidget(blankFrame);
      pulsantiList.append(stackedBox);

      griglia->addWidget(stackedBox,riga,col);
      connect(btn,SIGNAL(clicked()),this,SLOT(articoloSelezionato()));
      connect(repartoBtn,SIGNAL(cambiaColore(QColor)),btn,SLOT(setColore(QColor)));
      connect(repartoBtn,SIGNAL(cambiaFont(QFont)),btn,SLOT(setButtonFont(QFont)));
      connect(repartoBtn,SIGNAL(cambiaColoreText(QColor)),btn,SLOT(setColoreText(QColor)));

      //articoliBtnList.append(btn);
    }
  }
}

void MainWindow::creaInfoMessaggi()
{
  QStringList messaggi=QString("GESTIONE CASSA,build %1").arg(SVN_REV.c_str()).split(",");
  QString descrizione=confMap->value("descrManifestazione").toString();
  if(!descrizione.isEmpty()) {
    messaggi.insert(0,descrizione);
  }
  info->setListaTesto(messaggi);
}

void MainWindow::repartoSelezionato(){
  RepartoBtnWidget* btn=qobject_cast<RepartoBtnWidget*>(sender());
  //btn->setDown(true);
  ui->articoliStackedWidget->setCurrentIndex(btn->getId()-1);
  if(GESTIONE==modalitaCorrente) {
    dettagliRepartoBox->setCurrentReparto(btn);
    dettagliRepartoBox->disconnect();
    /*
    dettagliRepartoBox->show();
    dettagliArticoloBox->hide();
    */
    ui->latoStackedWidget->setCurrentWidget(dettagliRepartoBox);
  }
}

void MainWindow::articoloSelezionato(){
  ArticoloBtnWidget* btn=qobject_cast<ArticoloBtnWidget*>(sender());
  //btn->setDown(true);
  if(GESTIONE==modalitaCorrente) {
    dettagliArticoloBox->setCurrentArticolo(btn);
    /*
    dettagliArticoloBox->show();
    dettagliRepartoBox->hide();
    */
    ui->latoStackedWidget->setCurrentWidget(dettagliArticoloBox);
  } else {
    emit aggiungeArticolo(btn->getId(),btn->getNomeArticolo(),btn->getPrezzo());
  }
}

void MainWindow::on_configurazioneBtn_clicked()
{
  bool oldVisualizzaPrezzo=confMap->value("visualizzazionePrezzo").toBool();
  ConfigurazioneDlg* dlg=new ConfigurazioneDlg(confMap);
  connect(dlg,SIGNAL(resetOrdini(int)),ordineBox,SLOT(nuovoOrdine(int)));
  connect(dlg,SIGNAL(resetArticoli()),this,SLOT(creaRepartiButtons()));
  dlg->exec();

  bool newVisualizzaPrezzo=confMap->value("visualizzazionePrezzo").toBool();
  if(oldVisualizzaPrezzo!=newVisualizzaPrezzo) {
    QListIterator<QStackedWidget*> it(pulsantiList);
    while(it.hasNext()) {
      QStackedWidget* box=it.next();
      ArticoloBtnWidget* btnWidget=(ArticoloBtnWidget*)box->widget(0);
      btnWidget->setVisualizzaPrezzo(newVisualizzaPrezzo);
    }
  }

  creaInfoMessaggi();
}

void MainWindow::on_closeBtn_clicked()
{
    close();
}

void MainWindow::on_reportBtn_clicked()
{
  ReportForm* form=new ReportForm(confMap);
  form->exec();
}

void MainWindow::on_statsBtn_clicked()
{
    StatsForm* form=new StatsForm(confMap->value("sessioneCorrente").toInt());
  //form->setWindowState(Qt::WindowMaximized);
  form->exec();
}

void MainWindow::on_cassaBtn_clicked()
{
  //qApp->setOverrideCursor(Qt::BlankCursor);
  gestioneModalita(CASSA);
}

void MainWindow::on_gestioneBtn_clicked()
{
  if(ordineBox->isInComposizione()) {
    QMessageBox::information(this,"ATTENZIONE","Completare o annullare l'ordine corrente prima di cambiare modalità operativa");
    return;
  }
  qApp->restoreOverrideCursor();
  gestioneModalita(GESTIONE);
}
