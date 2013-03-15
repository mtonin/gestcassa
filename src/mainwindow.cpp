#include <QtGui>
#include <QMessageBox>
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

  connect(ui->modalitaBtn,SIGNAL(clicked()),this,SLOT(modalitaBtnClicked()));

  gestioneModalita(GESTIONE);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::gestioneModalita(const modalitaType nuovaModalita)
{
  if(GESTIONE==nuovaModalita) {
    ui->modalitaBtn->setText("CASSA");
    setWindowFlags(Qt::Window);
    ordineBox->setVisible(false);
    QListIterator<ArticoloBtnWidget*> btn(articoliBtnList);
    while(btn.hasNext()) {
      ArticoloBtnWidget* btnWidget=btn.next();
      btnWidget->setEnabled(true);
    }
    show();
  } else {
    ui->modalitaBtn->setText("GESTIONE");
    setWindowFlags(Qt::FramelessWindowHint);
    ordineBox->setVisible(true);
    dettagliRepartoBox->hide();
    dettagliArticoloBox->hide();
    QListIterator<ArticoloBtnWidget*> btn(articoliBtnList);
    while(btn.hasNext()) {
      ArticoloBtnWidget* btnWidget=btn.next();
      if(false==btnWidget->getAbilitato() || btnWidget->getNomeArticolo().isEmpty()) {
        btnWidget->setDisabled(true);
      }
    }
    show();
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
      btn->SetButtonColorHot(coloreSfondo);
      btn->setFont(currentFont);
      griglia->addWidget(btn,riga,col);
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

void MainWindow::repartoSelezionato(){
  RepartoBtnWidget* btn=qobject_cast<RepartoBtnWidget*>(sender());
  btn->setDown(true);
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
  btn->setDown(true);
  if(GESTIONE==modalitaCorrente) {
    dettagliArticoloBox->setCurrentArticolo(btn);
    dettagliArticoloBox->show();
    dettagliRepartoBox->hide();
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

void MainWindow::on_modalitaBtn_2_clicked()
{
  QFont font=qApp->font();
  font.setPointSize(font.pointSize()+1);
  qApp->setFont(font);

}
