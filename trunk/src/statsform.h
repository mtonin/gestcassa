#ifndef STATSFORM_H
#define STATSFORM_H

#include "ui_statsform.h"

class StatsForm : public QDialog, private Ui::StatsForm
{
  Q_OBJECT
  
public:
  explicit StatsForm(QWidget *parent = 0);
};

#endif // STATSFORM_H
