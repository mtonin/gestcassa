#include "restodlg.h"
#include <QTimer>
#include <QPropertyAnimation>

RestoDlg::RestoDlg(float valore,int durata,QWidget *parent) :QDialog(parent)
{
  setupUi(this);
  //setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowTitleHint);
  setWindowFlags(Qt::Tool);

  importoOrdine=valore;
  _durata=1000*durata;
  activateWindow();
  effetto=QSharedPointer<QPropertyAnimation>(new QPropertyAnimation(this,"windowOpacity"));

  if(_durata>0) {
    connect(effetto.data(),SIGNAL(finished()),this,SLOT(close()));
    effetto->setStartValue("1");
    effetto->setEndValue("0");
    effetto->setDuration(_durata);
    effetto->setEasingCurve(QEasingCurve::InQuint);
    effetto->start();
  }

  setFocus();
  importoTxt->setText(QString("%L1").arg(importoOrdine,4,'f',2));
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
  if(val.indexOf(',')>=0)
    return;
  val.append(",");
  importoRicevutoTxt->setText(val);
}
void RestoDlg::cancellaUltimoCarattere(){
  QString val=importoRicevutoTxt->text();
  val.chop(1);
  importoRicevutoTxt->setText(val);
}

void RestoDlg::ricalcolaResto()
{
  float totale=importoRicevutoTxt->text().toFloat();
  float resto=totale-importoOrdine;
  restoCalcolatoTxt->setText(QString("%L1").arg(resto,4,'f',2));

  if(_durata>0) {
    effetto->stop();
    effetto->start();
  }
}

void RestoDlg::keyPressEvent(QKeyEvent *evt)
{
  switch(evt->key()) {
    case Qt::Key_0: {
        on_tasto0Btn_clicked();
        break;
      }
    case Qt::Key_1: {
        on_tasto1Btn_clicked();
        break;
      }
    case Qt::Key_2: {
        on_tasto2Btn_clicked();
        break;
      }
    case Qt::Key_3: {
        on_tasto3Btn_clicked();
        break;
      }
    case Qt::Key_4: {
        on_tasto4Btn_clicked();
        break;
      }
    case Qt::Key_5: {
        on_tasto5Btn_clicked();
        break;
      }
    case Qt::Key_6: {
        on_tasto6Btn_clicked();
        break;
      }
    case Qt::Key_7: {
        on_tasto7Btn_clicked();
        break;
      }
    case Qt::Key_8: {
        on_tasto8Btn_clicked();
        break;
      }
    case Qt::Key_9: {
        on_tasto9Btn_clicked();
        break;
      }
    case Qt::Key_Comma:
    case Qt::Key_Period: {
        on_tastoVirgolaBtn_clicked();
        break;
      }
    case Qt::Key_Delete:
    case Qt::Key_Backspace: {
        cancellaUltimoCarattere();
        break;
      }

  }
}
