#ifndef DBPARAMDLG_H
#define DBPARAMDLG_H

#include "ui_dbparamdlg.h"

class DBParamDlg : public QDialog, private Ui::DBParamDlg
{
  Q_OBJECT

public:
  explicit DBParamDlg(QWidget *parent = 0);
  void setNomeFile(const QString val) {nomeFile=val;}
  void setDBLocalePath(const QString val) {dbLocalePathTxt->setText(val);}

private slots:
  void on_dbLocaleFlg_clicked();
  void on_dbReteFlg_clicked();

  void accept();

private:
  bool openDBRemoto();

private:
  QString nomeFile;

};

#endif // DBPARAMDLG_H
