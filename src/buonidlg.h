#ifndef BUONIDLG_H
#define BUONIDLG_H

#include "ui_buonidlg.h"
#include "buoniquerymodel.h"

class BuoniDlg : public QDialog, private Ui::BuoniDlg
{
  Q_OBJECT

public:
  explicit BuoniDlg(QMap<QString, QVariant>* par,QWidget *parent = 0);

private slots:
  void on_nuovoBuonoBtn_clicked();
  void on_tuttiBox_clicked();
  void on_nonAnnullatiBox_clicked();
  void aggiornaUI();

  void on_stampaBtn_clicked();

private:
  buoniQueryModel* modello;

  void stampa(const QTextDocument *doc, const QString descrReport, bool preview);
  void putHeader(QTextCursor cursore, const QString testo);
  void formattaTabella(QTextTable *tabella, QVector<QString> colWidth);

  QMap<QString, QVariant>* configurazione;

};

#endif // BUONIDLG_H
