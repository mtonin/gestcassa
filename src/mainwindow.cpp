#include <QtGui>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "repartobtnwidget.h"
#include "articolobtnwidget.h"
#include "dettaglireparto.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  showMaximized();

  creaRepartiButtons();

  dettagliRepartoBox=new dettagliReparto;
  dettagliRepartoBox->hide();
  dettagliArticoloBox=new DettagliArticolo;
  dettagliArticoloBox->hide();
  ordineBox=new Ordine;
  ordineBox->hide();

  ui->latoFrame->setLayout(new QVBoxLayout);
  ui->latoFrame->layout()->addWidget(dettagliRepartoBox);
  ui->latoFrame->layout()->addWidget(dettagliArticoloBox);
  ui->latoFrame->layout()->addWidget(ordineBox);

  /*
  //setWindowFlags(Qt::FramelessWindowHint|Qt::CustomizeWindowHint);
  //setContentsMargins(0,0,0,0);

  QVBoxLayout* vbox=new QVBoxLayout;
  //vbox->setContentsMargins(0,0,0,0);

  AreaLavoro* workArea=new AreaLavoro;
  vbox->addWidget(workArea);
  //workArea->setVisible(false);

  //ComandiBox* cmdBox=new ComandiBox(ui->comandiFrame);
  //vbox->addWidget(cmdBox);

  ComandiGroupBox* cmdBox=new ComandiGroupBox(ui->comandiFrame);

  QHBoxLayout* hbox=new QHBoxLayout;
  //hbox->setContentsMargins(0,0,0,0);
  ordineBox=new Ordine;
  dettaglioBox=new DettaglioRepartoDlg;
  dettaglioBox->setVisible(false);

  hbox->addLayout(vbox,4);

  QFrame* frameLaterale=new QFrame;
  frameLaterale->setFrameShape(QFrame::Box);
  QVBoxLayout* frameLateraleVBox=new QVBoxLayout;
  frameLaterale->setLayout(frameLateraleVBox);
  frameLateraleVBox->addWidget(ordineBox);
  frameLateraleVBox->addWidget(dettaglioBox);

  hbox->addWidget(frameLaterale,1);
  centralWidget()->setLayout(hbox);

  gestioneModalita(0);

  //connect(cmdBox,SIGNAL(dbCaricato()),workArea,SLOT(caricaDaDatabase()));
  */

  connect(ui->modalitaBtn,SIGNAL(clicked()),this,SLOT(modalitaBtnClicked()));

  gestioneModalita(GESTIONE);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::gestioneModalita(const modalitaType modalita )
{
  if(GESTIONE==modalita) {
      setWindowFlags(Qt::Window);

      ordineBox->setVisible(false);
      show();
    } else {
      setWindowFlags(Qt::FramelessWindowHint);
      ordineBox->setVisible(true);
      dettagliRepartoBox->setVisible(false);
      dettagliArticoloBox->hide();
      show();
    }

  currentMode=modalita;
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

void MainWindow::creaRepartiButtons(){

  QHBoxLayout* hboxLayout = new QHBoxLayout(ui->repartiGroupBox);
  hboxLayout->setSpacing(2);
  hboxLayout->setContentsMargins(11, 11, 11, 11);
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
  griglia->setSpacing(1);
  for(int riga=0;riga<5;riga++) {
      for(int col=0;col<6;col++) {
          ArticoloBtnWidget* btn=new ArticoloBtnWidget(repartoBtn->getId(),riga,col);
          btn->SetButtonColorNormal(coloreSfondo);
          btn->setFont(currentFont);
          griglia->addWidget(btn,riga,col);
          connect(btn,SIGNAL(clicked()),this,SLOT(articoloSelezionato()));
          connect(repartoBtn,SIGNAL(cambiaColore(QColor)),btn,SLOT(SetButtonColorNormal(QColor)));
          connect(repartoBtn,SIGNAL(cambiaFont(QFont)),btn,SLOT(setButtonFont(QFont)));
        }
    }
  QFrame* pagina=new QFrame;
  pagina->setLayout(griglia);

  ui->articoliStackedWidget->addWidget(pagina);

}

void MainWindow::repartoSelezionato(){
  RepartoBtnWidget* btn=qobject_cast<RepartoBtnWidget*>(sender());
  btn->setDown(true);
  ui->articoliStackedWidget->setCurrentIndex(btn->getId()-1);
  dettagliRepartoBox->setCurrentReparto(btn);
  dettagliRepartoBox->disconnect();
  dettagliRepartoBox->show();
  dettagliArticoloBox->hide();
}

void MainWindow::articoloSelezionato(){
  ArticoloBtnWidget* btn=qobject_cast<ArticoloBtnWidget*>(sender());
  btn->setDown(true);
  dettagliArticoloBox->setCurrentArticolo(btn);
  dettagliArticoloBox->show();
  dettagliRepartoBox->hide();
}

void MainWindow::modalitaBtnClicked(){
  QString testo=ui->modalitaBtn->text();
  if(0==testo.compare("CASSA")) {
      //qApp->setOverrideCursor(Qt::BlankCursor);
      ui->modalitaBtn->setText("GESTIONE");
      gestioneModalita(CASSA);
    } else {
      qApp->restoreOverrideCursor();
      ui->modalitaBtn->setText("CASSA");
      gestioneModalita(GESTIONE);
    }

}
