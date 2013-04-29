#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "repartobtnwidget.h"
#include "articolobtnwidget.h"
#include "dettaglireparto.h"
#include "configurazionedlg.h"
#include <QtGui>
#include <QMessageBox>

MainWindow::MainWindow(QMap<QString,QVariant>* configurazione,QWidget *parent) : confMap(configurazione),QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  showMaximized();

  creaRepartiButtons();

  dettagliRepartoBox=new DettagliReparto;
  dettagliArticoloBox=new DettagliArticolo;
  ordineBox=new Ordine(confMap);

  ui->latoFrame->setLayout(new QVBoxLayout);
  ui->latoFrame->layout()->addWidget(dettagliRepartoBox);
  ui->latoFrame->layout()->addWidget(dettagliArticoloBox);
  ui->latoFrame->layout()->addWidget(ordineBox);

  connect(ui->modalitaBtn,SIGNAL(clicked()),this,SLOT(modalitaBtnClicked()));
  connect(this,SIGNAL(aggiungeArticolo(int,QString,float)),ordineBox,SLOT(nuovoArticolo(int,QString,float)));
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
    ordineBox->hide();
    dettagliRepartoBox->hide();
    dettagliArticoloBox->hide();

    QListIterator<QStackedWidget*> it(stackedList);
    while(it.hasNext()) {
      it.next()->setCurrentIndex(0);
    }
    show();
  } else {
    ui->modalitaBtn->setText("GESTIONE");
    setWindowFlags(Qt::FramelessWindowHint);
    ordineBox->setVisible(true);
    dettagliRepartoBox->hide();
    dettagliArticoloBox->hide();
    QListIterator<QStackedWidget*> it(stackedList);
    while(it.hasNext()) {
      QStackedWidget* box=it.next();
      ArticoloBtnWidget* btnWidget=(ArticoloBtnWidget*)box->widget(0);
      if(!btnWidget->getAbilitato() || btnWidget->getNomeArticolo().isEmpty()) {
        box->setCurrentIndex(1);
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

  for(int riga=0;riga<5;riga++) {
    for(int col=0;col<6;col++) {
      QStackedWidget* stackedBox=new QStackedWidget();

      ArticoloBtnWidget* btn=new ArticoloBtnWidget(repartoBtn->getId(),riga,col);
      btn->SetButtonColorNormal(coloreSfondo);
      btn->SetButtonColorHot(coloreSfondo);
      btn->setFont(currentFont);
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

void MainWindow::on_modalitaBtn_2_clicked()
{
  QFont font=qApp->font();
  font.setPointSize(font.pointSize()+1);
  qApp->setFont(font);

}

void MainWindow::on_configurazioneBtn_clicked()
{
  ConfigurazioneDlg* dlg=new ConfigurazioneDlg(confMap);
  dlg->exec();
}

void MainWindow::on_closeBtn_clicked()
{
    close();
}
