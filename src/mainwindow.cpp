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
  setContentsMargins(0,0,0,0);

  QVBoxLayout* vbox=new QVBoxLayout;
  //vbox->setContentsMargins(0,0,0,0);

  AreaLavoro* workArea=new AreaLavoro;
  vbox->addWidget(workArea);

  ComandiBox* cmdBox=new ComandiBox;
  vbox->addWidget(cmdBox);

  QHBoxLayout* hbox=new QHBoxLayout;
  //hbox->setContentsMargins(0,0,0,0);
  ordineBox=new Ordine;
  hbox->addWidget(ordineBox);
  hbox->addLayout(vbox);

  centralWidget()->setLayout(hbox);

  gestioneModalita(1);

  connect(cmdBox,SIGNAL(dbCaricato()),workArea,SLOT(caricaDaDatabase()));
  connect(cmdBox,SIGNAL(cambiaModalita(int)),this,SLOT(gestioneModalita(int)));

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::gestioneModalita(const int modalita)
{
  if(1==modalita) {
      setWindowFlags(Qt::FramelessWindowHint);
      ordineBox->setVisible(true);
      show();
    } else {
      setWindowFlags(Qt::Window);
      ordineBox->setVisible(false);
      show();
    }
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
