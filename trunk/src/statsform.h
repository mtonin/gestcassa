#ifndef STATSFORM_H
#define STATSFORM_H

#include "ui_statsform.h"
#include <statsmodel.h>

class StatsForm : public QDialog, private Ui::StatsForm
{
  Q_OBJECT
  
public:
  explicit StatsForm(QWidget *parent = 0);

private slots:
  void on_filtraBtn_clicked();

private:
  StatsModel* statsModel;
  void caricaStats();
};

#endif // STATSFORM_H
