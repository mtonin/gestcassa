#ifndef DETTAGLIOREPARTODLG_H
#define DETTAGLIOREPARTODLG_H

#include <QFont>
#include <QColor>
#include "ui_dettagliorepartodlg.h"

class DettaglioRepartoDlg : public QDialog, private Ui::DettaglioRepartoDlg
{
  Q_OBJECT
  
public:
  explicit DettaglioRepartoDlg(QWidget *parent = 0);
  int showAtMousePosition();
  QString getDescrizione() {return lineEdit->text();}
  QFont getCurrentFont() {return currentFont;}
  QString getCurrentColor() {return currentColor;}
  void setCurrentFont(const QFont f) {currentFont=f;}
  void setCurrentColor(const QString c) {currentColor=c;}
  QString getTesto() {return lineEdit->text();}
  void setTesto(const QString s) {lineEdit->setText(s);}

private slots:
  void on_pushButton_clicked();
  void on_toolButton_clicked();

private:
  QFont currentFont;
  QString currentColor;
};

#endif // DETTAGLIOREPARTODLG_H
