#ifndef REPORTFORM_H
#define REPORTFORM_H

#include "ui_reportform.h"
#include <QTextCursor>

class ReportForm : public QWidget, private Ui::ReportForm
{
  Q_OBJECT
  
public:
  explicit ReportForm(QWidget *parent = 0);
private slots:
  void on_stampaBtn_clicked();

private:
  void stampaTutto(const QString& nomeFile);
  void stampaPerReparti(const QString& nomeFile);
  void stampaPerDestinazione(const QString& nomeFile);
  void putHeader(QTextCursor cursore, const QString testo);
};

#endif // REPORTFORM_H
