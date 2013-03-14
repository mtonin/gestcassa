#include "repartobtnwidget.h"
#include "dettagliorepartodlg.h"
#include <QtSql>
#include <QMessageBox>
#include <QAction>

RepartoBtnWidget::RepartoBtnWidget(QWidget *parent):
  QPictureButton(parent)
{
}

void RepartoBtnWidget::setColore(const QColor &colore)
{
  SetButtonColorNormal(colore);
  emit cambiaColore(colore);
}

void RepartoBtnWidget::setButtonFont(const QFont &font)
{
  //currentFont=font;
  setFont(font);
  emit cambiaFont(font);
}

RepartoBtnWidget::RepartoBtnWidget(int id,QWidget *parent) :
  idReparto(id),
  QPictureButton(parent)
{
  QSqlQuery stmt;
  stmt.prepare("select * from reparti where idreparto=?");
  stmt.addBindValue(idReparto);
  if(!stmt.exec()) {
    QMessageBox::critical(0, QObject::tr("Database Error"),
                          stmt.lastError().text());
    return;
  }
  int numColDescr=stmt.record().indexOf("descrizione");
  int numColFont=stmt.record().indexOf("font");
  int numColColoreSfondo=stmt.record().indexOf("coloresfondo");
  if(stmt.next()) {
    nomeReparto=stmt.value(numColDescr).toString();
    QFont currentFont;
    currentFont.fromString(stmt.value(numColFont).toString());
    setFont(currentFont);
    QString coloreSfondo=stmt.value(numColColoreSfondo).toString();
    SetButtonColorNormal(coloreSfondo);
  } else {
    nomeReparto=QString("REPARTO %1").arg(idReparto);
  }
  setText(nomeReparto);

  QSizePolicy buttonSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  buttonSizePolicy.setHorizontalStretch(0);
  buttonSizePolicy.setVerticalStretch(0);
  buttonSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
  setSizePolicy(buttonSizePolicy);

}

void RepartoBtnWidget::on_dettagliAction_triggered()
{

  DettaglioRepartoDlg dlg;
  dlg.setCurrentFont(font());
  dlg.setTesto(nomeReparto);
  //dlg.setCurrentColor(coloreSfondo.name());
  if(dlg.showAtMousePosition()) {

    QString coloreSfondo=dlg.getCurrentColor();
    QFont currentFont=dlg.getCurrentFont();

    QSqlQuery stmt;
    stmt.prepare("update reparti set descrizione=?,font=?,coloresfondo=? where idreparto=?");
    stmt.addBindValue(dlg.getDescrizione());
    stmt.addBindValue(currentFont.toString());
    stmt.addBindValue(coloreSfondo);
    stmt.addBindValue(idReparto);
    if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            stmt.lastError().text());
      return;
    }

    setText(dlg.getDescrizione());
    setFont(currentFont);

    SetButtonColorNormal(QColor(coloreSfondo));
    QString stile=QString("QPushButton {background-color: %1;}").arg(coloreSfondo);
    setStyleSheet(stile);
    emit cambiaColore(coloreSfondo);
  }
}
