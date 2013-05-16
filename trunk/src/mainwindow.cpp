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

#include <QtGui>
#include <QMessageBox>
#include <QHBoxLayout>

MainWindow::MainWindow(QMap<QString,QVariant>* configurazione,QWidget *parent) : confMap(configurazione),QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  setWindowFlags(Qt::FramelessWindowHint);

  showMaximized();

  creaRepartiButtons();

  dettagliRepartoBox=new DettagliReparto;
  dettagliArticoloBox=new DettagliArticolo;
  ordineBox=new Ordine(confMap);

  ui->latoFrame->setLayout(new QVBoxLayout);
  ui->latoFrame->layout()->addWidget(dettagliRepartoBox);
  ui->latoFrame->layout()->addWidget(dettagliArticoloBox);
  ui->latoFrame->layout()->addWidget(ordineBox);

  connect(this,SIGNAL(aggiungeArticolo(int,QString,float)),ordineBox,SLOT(nuovoArticolo(int,QString,float)));

  QDigitalClock* orologio=new QDigitalClock;
  orologio->SetFormat("dd-MM-yyyy\nHH:mm:ss");
  QFont font=orologio->font();
  font.setBold(true);
  font.setPointSize(14);
  orologio->setFont(font);
  //orologio->SetTextColor(Qt::red);
  orologio->SetAlignment(Qt::AlignHCenter);

  ui->clockFrame->setLayout(new QHBoxLayout);
  ui->clockFrame->layout()->addWidget(orologio);

  QStringList messaggi=QString("GESTIONE CASSA,build %1").arg(SVN_REV.c_str()).split(",");
  QString descrizione=confMap->value("descrManifestazione").toString();
  if(!descrizione.isEmpty()) {
    messaggi.insert(0,descrizione);
  }
  info=new infoWidget(messaggi);

  QHBoxLayout* infoLayout=new QHBoxLayout;
  infoLayout->setContentsMargins(QMargins(0,0,0,0));
  infoLayout->addWidget(info);
  ui->infoFrame->setLayout(infoLayout);

  if("operatore"==confMap->value("ruolo","operatore")) {
    ui->modalitaBtn->setEnabled(false);
    ui->configurazioneBtn->setEnabled(false);
    ui->reportBtn->setEnabled(false);
    gestioneModalita(CASSA);
  } else {
    connect(ui->modalitaBtn,SIGNAL(clicked()),this,SLOT(modalitaBtnClicked()));
    gestioneModalita(GESTIONE);
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::gestioneModalita(const modalitaType nuovaModalita)
{
  if(GESTIONE==nuovaModalita) {
    ui->modalitaBtn->setText("CASSA");
    ui->modalitaBtn->setIcon(QIcon(":/GestCassa/cassa"));
    ordineBox->hide();
    dettagliRepartoBox->hide();
    dettagliArticoloBox->hide();

    QListIterator<QStackedWidget*> it(stackedList);
    while(it.hasNext()) {
      it.next()->setCurrentIndex(0);
    }
    showMaximized();
  } else {
    ui->modalitaBtn->setText("GESTIONE");
    ui->modalitaBtn->setIcon(QIcon(":/GestCassa/gestione"));
    dettagliRepartoBox->hide();
    dettagliArticoloBox->hide();
    ordineBox->setVisible(true);
    QListIterator<QStackedWidget*> it(stackedList);
    while(it.hasNext()) {
      QStackedWidget* box=it.next();
      ArticoloBtnWidget* btnWidget=(ArticoloBtnWidget*)box->widget(0);
      if(!btnWidget->getAbilitato() || btnWidget->getNomeArticolo().isEmpty()) {
        box->setCurrentIndex(1);
      }
    }
    showMaximized();
  }

  modalitaCorrente=nuovaModalita;
}

void MainWindow::keyPressEvent(QKeyEvent *evt) {
  switch(evt->key()) {
    case Qt::Key_F11: {
        if(isMaximized())
          showNormal();
        else
          showMaximized();
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

  QHBoxLayout* hboxLayout = new QHBoxLayout(ui->repartiGroupBox);
  hboxLayout->setSpacing(2);
  hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
  hboxLayout->setContentsMargins(-1, 5, -1, 5);
  for(int i=1;i<=6;i++) {
    RepartoBtnWidget* reparto01Btn = new RepartoBtnWidget(i,ui->repartiGroupBox);

    hboxLayout->addWidget(reparto01Btn);
    creaArticoliPerRepartoButtons(reparto01Btn);

    connect(reparto01Btn,SIGNAL(clicked()),this,SLOT(repartoSelezionato()));
  }

  ui->repartiGroupBox->setLayout(hboxLayout);
}


void MainWindow::creaArticoliPerRepartoButtons(RepartoBtnWidget* repartoBtn)   {

  QColor coloreSfondo=repartoBtn->buttonColorNormal();
  QFont currentFont=repartoBtn->getFont();
  QGridLayout* griglia=new QGridLayout;
  griglia->setSpacing(2);

  bool visualizzaPrezzo=confMap->value("visualizzazionePrezzo").toBool();
  for(int riga=0;riga<5;riga++) {
    for(int col=0;col<6;col++) {
      QStackedWidget* stackedBox=new QStackedWidget();

      ArticoloBtnWidget* btn=new ArticoloBtnWidget(repartoBtn->getId(),riga,col);
      btn->SetButtonColorNormal(coloreSfondo);
      btn->SetButtonColorHot(coloreSfondo);
      btn->setFont(currentFont);
      btn->setVisualizzaPrezzo(visualizzaPrezzo);
      stackedBox->addWidget(btn);
      QFrame* blankFrame=new QFrame;
      stackedBox->addWidget(blankFrame);
      stackedList.append(stackedBox);

      griglia->addWidget(stackedBox,riga,col);
      connect(btn,SIGNAL(clicked()),this,SLOT(articoloSelezionato()));
      connect(repartoBtn,SIGNAL(cambiaColore(QColor)),btn,SLOT(SetButtonColorNormal(QColor)));
      connect(repartoBtn,SIGNAL(cambiaFont(QFont)),btn,SLOT(setButtonFont(QFont)));
      articoliBtnList.append(btn);
    }
  }
  QFrame* pagina=new QFrame;
  pagina->setLayout(griglia);

  ui->articoliStackedWidget->addWidget(pagina);

}

void MainWindow::creaInfoMessaggi()
{
  QStringList messaggi=QString("GESTIONE CASSA,build %1").arg(SVN_REV.c_str()).split(",");
  QString descrizione=confMap->value("descrManifestazione").toString();
  if(!descrizione.isEmpty()) {
    messaggi.insert(0,descrizione);
  }
  info->setlistaTesto(messaggi);
}

void MainWindow::repartoSelezionato(){
  RepartoBtnWidget* btn=qobject_cast<RepartoBtnWidget*>(sender());
  //btn->setDown(true);
  ui->articoliStackedWidget->setCurrentIndex(btn->getId()-1);
  if(GESTIONE==modalitaCorrente) {
    dettagliRepartoBox->setCurrentReparto(btn);
    dettagliRepartoBox->disconnect();
    dettagliRepartoBox->show();
    dettagliArticoloBox->hide();
  }
}

void MainWindow::articoloSelezionato(){
  ArticoloBtnWidget* btn=qobject_cast<ArticoloBtnWidget*>(sender());
  //btn->setDown(true);
  if(GESTIONE==modalitaCorrente) {
    dettagliArticoloBox->setCurrentArticolo(btn);
    dettagliArticoloBox->show();
    dettagliRepartoBox->hide();
  } else {
    emit aggiungeArticolo(btn->getId(),btn->getNomeArticolo(),btn->getPrezzo());
  }
}

void MainWindow::modalitaBtnClicked(){
  if(GESTIONE==modalitaCorrente) {
    //qApp->setOverrideCursor(Qt::BlankCursor);
    gestioneModalita(CASSA);
  } else {
    qApp->restoreOverrideCursor();
    gestioneModalita(GESTIONE);
  }

}

void MainWindow::on_configurazioneBtn_clicked()
{
  bool oldVisualizzaPrezzo=confMap->value("visualizzazionePrezzo").toBool();
  ConfigurazioneDlg* dlg=new ConfigurazioneDlg(confMap);
  dlg->exec();

  bool newVisualizzaPrezzo=confMap->value("visualizzazionePrezzo").toBool();
  if(oldVisualizzaPrezzo!=newVisualizzaPrezzo) {
    QListIterator<QStackedWidget*> it(stackedList);
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
