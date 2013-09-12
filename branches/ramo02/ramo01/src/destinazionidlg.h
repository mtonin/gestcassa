#ifndef DESTINAZIONIDLG_H
#define DESTINAZIONIDLG_H

#include "ui_destinazionidlg.h"
#include <QSqlTableModel>
#include <QDataWidgetMapper>

class DestinazioniDlg : public QWidget, private Ui::DestinazioniDlg
{
  Q_OBJECT
  
public:
  explicit DestinazioniDlg(QWidget *parent = 0);
  ~DestinazioniDlg();

private slots:

  void on_cancellaBtn_clicked();

  void on_nuovoBtn_clicked();

private:
  QSqlTableModel* modello;
  QDataWidgetMapper* mapper;

};

#endif // DESTINAZIONIDLG_H
