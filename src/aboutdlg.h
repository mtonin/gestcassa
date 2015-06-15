#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include "ui_aboutdlg.h"

class AboutDlg : public QDialog, private Ui::AboutDlg
{
  Q_OBJECT

public:
  explicit AboutDlg(QWidget *parent = 0);
};

#endif // ABOUTDLG_H
