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

  void on_cartellaPdfTxt_textEdited(const QString &arg1);

  void on_stampanteSelezionataTxt_textEdited(const QString &arg1);

  void on_nomeCassaTxt_textEdited(const QString &arg1);

  void on_durataRestoTxt_textEdited(const QString &arg1);

  void on_descrManifestazioneTxt_textEdited(const QString &arg1);

  void on_adminPasswordTxt_textEdited(const QString &arg1);

  void on_intestazioneScontrinoTxt_textChanged();

  void on_visualizzaPrezzoBox_clicked(bool checked);

private:
  QMap<QString,QVariant>* configurazione;
  QMap<QString,QVariant>* nuovaConfigurazione;
  SimpleCrypt* cifratore;
};

#endif // CONFIGURAZIONEDLG_H
