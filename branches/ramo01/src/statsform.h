#ifndef STATSFORM_H
#define STATSFORM_H

#include "ui_statsform.h"
#include <statsmodel.h>

class StatsForm : public QDialog, private Ui::StatsForm
{
  Q_OBJECT
  
public:
  explicit StatsForm(const int, QWidget *parent = 0);

private slots:
  void on_filtraBtn_clicked();
  void ordinaByColumn(int column);

private:
  StatsModel* statsModel;
  void caricaStats();
  void calcolaTotali();

  int idSessioneCorrente;

};

#endif // STATSFORM_H
