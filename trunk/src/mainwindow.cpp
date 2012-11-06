#include <QtGui>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ordine.h"
#include "comandibox.h"
#include "arealavoro.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  //setWindowFlags(Qt::FramelessWindowHint|Qt::CustomizeWindowHint);

  QVBoxLayout* vbox=new QVBoxLayout;
  AreaLavoro* workArea=new AreaLavoro;
  vbox->addWidget(workArea);

  ComandiBox* cmdBox=new ComandiBox;
  vbox->addWidget(cmdBox);

  QHBoxLayout* hbox=new QHBoxLayout;
  hbox->addWidget(new Ordine);
  hbox->addLayout(vbox);

  centralWidget()->setLayout(hbox);

  connect(cmdBox,SIGNAL(dbCaricato()),workArea,SLOT(caricaDaDatabase()));
  /*
  connect(cmdBox,SIGNAL(cambiaModalita(int)),this,SLOT(gestioneModalita(int)));

  connect(repBox,SIGNAL(onCancellazioneReparto()),this,SLOT(repartoSlot()));
  connect(repBox,SIGNAL(onSelezioneReparto(RepartoButton*)),this,SLOT(repartoSlot(RepartoButton*)));
  */

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::gestioneModalita(const int modalita)
{
  if(1==modalita) {
      setWindowFlags(Qt::FramelessWindowHint);
      show();
    } else {
      setWindowFlags(Qt::Window);
      show();
    }
}

void MainWindow::repartoSlot(RepartoButton* btn)
{
  QString nomeReparto(btn->getNomeReparto());
  QMessageBox::information(0,"SLOT",nomeReparto);
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
