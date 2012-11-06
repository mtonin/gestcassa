#include "repartobtnwidget.h"
#include "dettagliorepartodlg.h"
#include <QtSql>
#include <QMessageBox>
#include <QAction>

RepartoBtnWidget::RepartoBtnWidget(int id,QString nome,QWidget *parent) :
  idReparto(id),
  nomeReparto(nome),
  QPushButton(parent)
{
  QSqlDatabase db=QSqlDatabase::database();
  QSqlQuery stmt;
  stmt.prepare("select * from reparti where idreparto=?");
  stmt.addBindValue(idReparto);
  if(!stmt.exec()) {
      QMessageBox::critical(0, QObject::tr("Database Error"),
                            db.lastError().text());
      return;
    }
  int numColDescr=stmt.record().indexOf("descrizione");
  int numColFont=stmt.record().indexOf("font");
  int numColColoreSfondo=stmt.record().indexOf("coloresfondo");
  if(stmt.next()) {
      setText(stmt.value(numColDescr).toString());
      QFont currentFont;
      currentFont.fromString(stmt.value(numColFont).toString());
      setFont(currentFont);
      coloreSfondo=stmt.value(numColColoreSfondo).toString();
      QString stile=QString("QPushButton {background-color: %1;}").arg(coloreSfondo);
      setStyleSheet(stile);
    }
  else {
      setText(nomeReparto);
    }

  QAction* dettagliAction=new QAction("Modifica",this);
  dettagliAction->setObjectName("dettagliAction");
  addAction(dettagliAction);

  QAction* cancellaAction=new QAction("Cancella",this);
  cancellaAction->setObjectName("cancellaAction");
  addAction(cancellaAction);
  setContextMenuPolicy(Qt::ActionsContextMenu);

  QSizePolicy buttonSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  buttonSizePolicy.setHorizontalStretch(0);
  buttonSizePolicy.setVerticalStretch(0);
  buttonSizePolicy.setHeightForWidth(sizePolicy().hasHeightForWidth());
  setSizePolicy(buttonSizePolicy);

  QMetaObject::connectSlotsByName(this);
}

void RepartoBtnWidget::on_dettagliAction_triggered()
{

  DettaglioRepartoDlg dlg;
  dlg.setCurrentFont(font());
  dlg.setTesto(nomeReparto);
  dlg.setCurrentColor(coloreSfondo);
  if(dlg.showAtMousePosition()) {

    QString coloreSfondo=dlg.getCurrentColor();
    QFont currentFont=dlg.getCurrentFont();

    QSqlDatabase db=QSqlDatabase::database();
    QSqlQuery stmt;
    stmt.prepare("update reparti set descrizione=?,font=?,coloresfondo=? where idreparto=?");
    stmt.addBindValue(dlg.getDescrizione());
    stmt.addBindValue(currentFont.toString());
    stmt.addBindValue(coloreSfondo);
    stmt.addBindValue(idReparto);
    if(!stmt.exec()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              db.lastError().text());
        return;
      }

    setText(dlg.getDescrizione());
    setFont(currentFont);

    QString stile=QString("QPushButton {background-color: %1;}").arg(coloreSfondo);
    setStyleSheet(stile);
  }
}

void RepartoBtnWidget::on_cancellaAction_triggered()
{
  emit cancellaReparto(this);
}
