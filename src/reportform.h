#ifndef REPORTFORM_H
#define REPORTFORM_H

#include "ui_reportform.h"
#include <QTextCursor>
#include <QMap>
class ReportForm : public QWidget, private Ui::ReportForm
{
  Q_OBJECT
  
public:
  explicit ReportForm(QMap<QString,QVariant>* par,QWidget *parent = 0);
private slots:
  void stampa(bool preview);

  void on_anteprimaBtn_clicked();

  void on_esportaBtn_clicked();

private:
  QTextDocument* creaDocumentTutto();
  QTextDocument* creaDocumentPerReparti();
  QTextDocument* creaDocumentPerDestinazione();
  void stampa(const QTextDocument* doc,const QString descrReport,bool preview);
  void putHeader(QTextCursor cursore, const QString testo);

  QMap<QString,QVariant>* configurazione;

};

#endif // REPORTFORM_H
