#include "restodlg.h"
#include <QTimer>
#include <QPropertyAnimation>

RestoDlg::RestoDlg(float valore,int durata,QWidget *parent) :QDialog(parent)
{
  setupUi(this);
  _durata=1000*durata;
  setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
  activateWindow();
  effetto=new QPropertyAnimation(this,"windowOpacity");

  if(_durata>0) {
    connect(effetto,SIGNAL(finished()),this,SLOT(close()));
    effetto->setStartValue("1");
    effetto->setEndValue("0");
    effetto->setDuration(_durata);
    effetto->start();
  }

  importoTxt->setText(QString("%1").arg(valore));
  connect(importoRicevutoTxt,SIGNAL(textChanged(QString)),this,SLOT(ricalcolaResto()));
  ricalcolaResto();
}

RestoDlg::~RestoDlg()
{
}

void RestoDlg::on_tasto0Btn_clicked(){
  if(!importoRicevutoTxt->text().isEmpty())
    importoRicevutoTxt->setText(importoRicevutoTxt->text().append("0"));
}
void RestoDlg::on_tasto1Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("1"));
}
void RestoDlg::on_tasto2Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("2"));
}
void RestoDlg::on_tasto3Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("3"));
}
void RestoDlg::on_tasto4Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("4"));
}
void RestoDlg::on_tasto5Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("5"));
}
void RestoDlg::on_tasto6Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("6"));
}
void RestoDlg::on_tasto7Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("7"));
}
void RestoDlg::on_tasto8Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("8"));
}
void RestoDlg::on_tasto9Btn_clicked(){
  importoRicevutoTxt->setText(importoRicevutoTxt->text().append("9"));
}
void RestoDlg::on_tastoVirgolaBtn_clicked(){
  QString val=importoRicevutoTxt->text();
  if(val.isEmpty())
    val="0";
  if(val.indexOf('.')>=0)
    return;
  val.append(".");
  importoRicevutoTxt->setText(val);
}

void RestoDlg::ricalcolaResto()
{
  float totale=importoRicevutoTxt->text().toFloat();
  float importoDaPagare=importoTxt->text().toFloat();
  float resto=totale-importoDaPagare;
  restoCalcolatoTxt->setText(QString("%1").arg(resto));

  if(_durata>0) {
    effetto->stop();
    effetto->start();
  }
}
