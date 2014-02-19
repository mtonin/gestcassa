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
#include "confermadlg.h"

#include <QtGui>
#include <QMessageBox>
#include <QHBoxLayout>

const int NUM_REPARTI=8;
const int NUM_RIGHE_ART=5;
const int NUM_COLONNE_ART=6;

MainWindow::MainWindow(QMap<QString,QVariant>* configurazione,QWidget *parent) : confMap(configurazione),QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  setAcceptDrops(true);

  cifratore=new SimpleCrypt(Q_UINT64_C(0x529c2c1779964f9d));
  decodificaPassword();

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

  QStringList messaggi=QString("").split(",");
  info=new infoWidget(messaggi);
  creaInfoMessaggi();

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
  delete cifratore;
}

void MainWindow::gestioneModalita(const modalitaType nuovaModalita)
{
  if(GESTIONE==nuovaModalita) {

    ConfermaDlg dlg ("Inserire la password per accedere alla modalità amministrativa.","Password",true);
    while(true) {
      if(QDialog::Accepted!=dlg.visualizza()) return;
      if(adminPassword==dlg.getValore()) {
        break;
      }
      QMessageBox::critical(this,"Accesso","Password errata");
    }

    ui->configurazioneBtn->setEnabled(true);
    ui->reportBtn->setEnabled(true);
    ui->cassaBtn->setEnabled(true);
    ui->gestioneBtn->setEnabled(false);
    ui->latoStackedWidget->setCurrentIndex(0);

    // attiva tutti i pulsanti dei reparti
    QListIterator<RepartoBtnWidget*> itReparti(repartiList);
    while(itReparti.hasNext()) {
      itReparti.next()->setVisible(true);
      //itReparti.next()->setEnabled(true);
    }
    QListIterator<QStackedWidget*> it(articoliList);
    while(it.hasNext()) {
      QStackedWidget* box=it.next();
      box->setCurrentIndex(0);
      ArticoloBtnWidget* btnWidget=(ArticoloBtnWidget*)box->widget(0);
      //abilita drag & drop
      btnWidget->setDragAbilitato(true);
    }

    ui->articoliStackedWidget->setAcceptDrops(true);

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
      reparto->setVisible(reparto->getAbilitato());
      //reparto->setEnabled(reparto->getAbilitato());
      if(primoRepartoAttivo<0 && reparto->getAbilitato()) primoRepartoAttivo=reparto->getId();
    }

    // nasconde i pulsanti degli articoli disabilitati
    QListIterator<QStackedWidget*> it(articoliList);
    while(it.hasNext()) {
      QStackedWidget* box=it.next();
      ArticoloBtnWidget* btnWidget=(ArticoloBtnWidget*)box->widget(0);
      //disabilita drag & drop
      btnWidget->setDragAbilitato(false);
      if(!btnWidget->getAbilitato() || btnWidget->getNomeArticolo().isEmpty()) {
        box->setCurrentIndex(1);
      }
    }

    ui->articoliStackedWidget->setAcceptDrops(false);

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
  ConfermaDlg dlg("Confermi l'uscita?");
  if(QDialog::Accepted!=dlg.visualizza()) evt->ignore();
}

void MainWindow::creaRepartiButtons(){

  // cancella i pulsanti degli articoli
  QListIterator<QStackedWidget*> itArticoli(articoliList);
  while(itArticoli.hasNext()) {
    QStackedWidget* articolo=itArticoli.next();
    delete articolo;
  }
  articoliList.clear();

  // cancella i pulsanti dei reparti
  QListIterator<RepartoBtnWidget*> itReparti(repartiList);
  while(itReparti.hasNext()) {
    RepartoBtnWidget* reparto=itReparti.next();
    delete reparto;
  }
  repartiList.clear();

  QLayout* hboxLayout = ui->repartiBox->layout();
  if(!hboxLayout) {
    hboxLayout = new QHBoxLayout(ui->repartiBox);
    hboxLayout->setSpacing(2);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(-1, 5, -1, 5);
  }

  while(ui->articoliStackedWidget->count()>0) {
    QWidget* w=ui->articoliStackedWidget->widget(0);
    ui->articoliStackedWidget->removeWidget(w);
  }

  for(int i=0;i<NUM_REPARTI;i++) {
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

      QStackedWidget* stackedBox=new QStackedWidget;
      int idPulsante=numReparto*NUM_RIGHE_ART*NUM_COLONNE_ART+riga*NUM_COLONNE_ART+col;

      ArticoloBtnWidget* btn=new ArticoloBtnWidget(idPulsante,repartoBtn->getId(),riga,col);
      btn->SetButtonColorNormal(coloreSfondo);
      btn->SetButtonColorHot(coloreSfondo);
      btn->SetTextColorEnabled(coloreCarattere);
      btn->setFont(currentFont);
      btn->setVisualizzaPrezzo(visualizzaPrezzo);
      connect(btn,SIGNAL(swapSignal(int,int)),this,SLOT(scambia(int,int)));
      stackedBox->addWidget(btn);
      QFrame* blankFrame=new QFrame;
      stackedBox->addWidget(blankFrame);
      articoliList.append(stackedBox);

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
  QStringList messaggi=QString("GESTIONE CASSA,versione %1").arg(VERSIONE.c_str()).split(",");
  QString descrizione=confMap->value("descrManifestazione").toString();
  if(!descrizione.isEmpty()) {
    messaggi.insert(0,descrizione);
  }
  info->setListaTesto(messaggi);
}

void MainWindow::repartoSelezionato(){
  RepartoBtnWidget* btn=qobject_cast<RepartoBtnWidget*>(sender());
  //btn->setDown(true);
  ui->articoliStackedWidget->setCurrentIndex(btn->getId());
  if(GESTIONE==modalitaCorrente) {
    dettagliRepartoBox->setCurrentReparto(btn);
    dettagliRepartoBox->disconnect();
    ui->latoStackedWidget->setCurrentWidget(dettagliRepartoBox);
  }
}

void MainWindow::articoloSelezionato(){
  ArticoloBtnWidget* btn=qobject_cast<ArticoloBtnWidget*>(sender());
  //btn->setDown(true);
  if(GESTIONE==modalitaCorrente) {
    dettagliArticoloBox->setCurrentArticolo(btn);
    ui->latoStackedWidget->setCurrentWidget(dettagliArticoloBox);
  } else {
    emit aggiungeArticolo(btn->getId(),btn->getNomeArticolo(),btn->getPrezzo());
  }
}

void MainWindow::on_configurazioneBtn_clicked()
{
  ConfigurazioneDlg* dlg=new ConfigurazioneDlg(confMap);
  connect(dlg,SIGNAL(resetOrdini(int)),ordineBox,SLOT(nuovoOrdine(int)));
  connect(dlg,SIGNAL(resetArticoli()),this,SLOT(creaRepartiButtons()));
  connect(dlg,SIGNAL(passwordCambiata()),this,SLOT(decodificaPassword()));
  connect(dlg,SIGNAL(cambiaVisualizzaPrezzo(bool)),this,SLOT(visualizzaPrezzo(bool)));
  dlg->exec();

  creaInfoMessaggi();
  delete dlg;
}

void MainWindow::on_closeBtn_clicked()
{
    close();
}

void MainWindow::on_reportBtn_clicked()
{
  ReportForm form(confMap);
  form.exec();
}

void MainWindow::on_statsBtn_clicked()
{
  StatsForm form(confMap->value("sessioneCorrente").toInt());
  //form->setWindowState(Qt::WindowMaximized);
  form.exec();
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

void MainWindow::decodificaPassword()
{
  adminPassword=confMap->value("adminPassword").toString();
  if(adminPassword.isEmpty()) {
    adminPassword="12345";
  } else {
    adminPassword=cifratore->decryptToString(adminPassword);
  }
}

void MainWindow::visualizzaPrezzo(bool visualizza)
{
  QListIterator<QStackedWidget*> it(articoliList);
  while(it.hasNext()) {
    QStackedWidget* box=it.next();
    ArticoloBtnWidget* btnWidget=(ArticoloBtnWidget*)box->widget(0);
    btnWidget->setVisualizzaPrezzo(visualizza);
  }
}

void MainWindow::scambia(int id1, int id2)
{
  QString msg=QString("Scambia %1 e %2").arg(id1).arg(id2);
  qDebug(msg.toAscii());

  QStackedWidget* box1=articoliList.value(id1);
  ArticoloBtnWidget* btnWidget1=(ArticoloBtnWidget*)box1->widget(0);
  int w1riga=btnWidget1->getRiga();
  int w1colonna=btnWidget1->getColonna();
  QStackedWidget* box2=articoliList.value(id2);
  ArticoloBtnWidget* btnWidget2=(ArticoloBtnWidget*)box2->widget(0);
  int w2riga=btnWidget2->getRiga();
  int w2colonna=btnWidget2->getColonna();

  QGridLayout* griglia=(QGridLayout*)ui->articoliStackedWidget->currentWidget()->layout();
  griglia->addWidget(box1,w2riga,w2colonna);
  griglia->addWidget(box2,w1riga,w1colonna);
  btnWidget1->setPos(w2riga,w2colonna);
  btnWidget2->setPos(w1riga,w1colonna);

  dettagliArticoloBox->setCurrentArticolo(btnWidget1);
  ui->latoStackedWidget->setCurrentWidget(dettagliArticoloBox);

}

void MainWindow::on_testBtn_clicked() {
    int idSessione=confMap->value("sessioneCorrente").toInt();
    if(idSessione<999999) {
        confMap->insert("sessioneCorrente",999999);
        confMap->insert("sessioneSalvata",idSessione);
        ordineBox->nuovoOrdine(999999);
    } else {
        idSessione=confMap->value("sessioneSalvata").toInt();
        confMap->insert("sessioneCorrente",idSessione);
        confMap->remove("sessioneSalvata");
        ordineBox->nuovoOrdine(idSessione);
    }
    gestioneModalita(CASSA);
}
