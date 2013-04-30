#ifndef CONFIGURAZIONEDLG_H
#define CONFIGURAZIONEDLG_H

#include "simplecrypt.h"
#include "ui_configurazionedlg.h"
#include <QMap>

class ConfigurazioneDlg : public QDialog, private Ui::ConfigurazioneDlg
{
  Q_OBJECT
  
public:
  explicit ConfigurazioneDlg(QMap<QString,QVariant>* par,QWidget *parent = 0);
  QMap<QString,QVariant> getConfigurazione() {return *configurazione;}

private slots:
  void on_printerSelectBtn_clicked();

  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

  void on_attivaRestoCheck_toggled(bool checked);

  void on_stampanteChk_toggled(bool checked);

  void on_pdfChk_toggled(bool checked);

  void on_cartellaPdfBtn_clicked();

  void on_cartellaPdfTxt_textChanged(const QString &arg1);

  void on_stampanteSelezionataTxt_textChanged(const QString &arg1);

  void on_intestazione1Txt_textChanged(const QString &arg1);

  void on_intestazione2Txt_textChanged(const QString &arg1);

  void on_intestazione3Txt_textChanged(const QString &arg1);

  void on_nomeCassaTxt_textChanged(const QString &arg1);

  void on_durataRestoTxt_textChanged(const QString &arg1);

  void on_descrManifestazioneTxt_textChanged(const QString &arg1);

  void on_adminPasswordTxt_textChanged(const QString &arg1);

private:
  QMap<QString,QVariant>* configurazione;
  QMap<QString,QVariant>* nuovaConfigurazione;
  SimpleCrypt* cifratore;
};

#endif // CONFIGURAZIONEDLG_H
