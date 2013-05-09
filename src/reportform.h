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
  void on_stampaBtn_clicked();

private:
  void stampaTutto();
  void stampaPerReparti();
  void stampaPerDestinazione();
  void creaPdf(const QTextDocument* doc,const QString descrReport);
  void putHeader(QTextCursor cursore, const QString testo);

  QMap<QString,QVariant>* configurazione;

};

#endif // REPORTFORM_H
